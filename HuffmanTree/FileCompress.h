
#include<stdlib.h>
#include<string>
#include<iostream>
using namespace std;
typedef unsigned long LongType;
//�ַ���Ϣ��Ϊ��������T���� ����Ҫ����һЩ����
struct CharInfo
{
	unsigned char _ch;  //�ַ�
	LongType _count;  //�ַ����ļ����ֵĴ���
	string _code;   //�ַ��Ĺ���������

	CharInfo()
		:_ch(0)
		,_count(0)
	{}

	CharInfo(unsigned char ch)
		:_ch(ch)
		, _count(0)
	{}

	bool operator!=(const CharInfo & ci) const
	{
		return this->_count != ci._count;
	}

	bool operator< (const CharInfo & ci)
	{
		return this->_count < ci._count;
	}

	CharInfo operator+(const CharInfo & ci)
	{
		CharInfo tmp(_ch);  //����������ֵ
		tmp._count = this->_count + ci._count;
		return tmp;
	}
};


class FileCompress
{
public:
	FileCompress()
	{
		for (int i = 0; i < 256; ++i)
		{
			_infos[i] = (unsigned char)i;
		}
	}

	//ѹ��
	bool Compress(const char * filename)
	{
		//����
		FILE * fp = fopen(filename, "r");
		if (!fp)
			return false;
		int ch;
		while ((ch = fgetc(fp)) != EOF)
		{
			++_infos[ch]._count;
		}

		//����HuffmanTree
		HuffmanTree<CharInfo> hf;
		CharInfo invalid;
		invalid._count = 0;
		hf.CreateHuffmanTree(_infos, 256, invalid);

		//��_code�������  ��Ҫ�ݹ�

		hf.CountCode(_infos);
		
#ifdef _DEBUG_
		Print();
#endif
		//���ļ� д���ļ�
		string OutFileName = string(filename) + ".huffcom";

		FILE* fOut = fopen(OutFileName.c_str(), "w");
		fseek(fp, 0, SEEK_SET);  //

		int pos = 0;  //����λ��
		unsigned char OutValue = 0;  //д���ֵ
		long long count = 0;

		while ((ch = fgetc(fp)) && !feof(fp))   
		{
			++count;
			string & tmp = _infos[ch]._code;

			//cout << tmp;

			for (auto it = tmp.begin(); it != tmp.end(); ++it)
			{
				OutValue <<= 1;
			    if (*it == '1')
				{	
					OutValue |= (unsigned char)1;
				}
				++pos;
				
				if (pos == 8)
				{
					fputc(OutValue, fOut);
					pos = 0;
					OutValue = 0;
					
				}
			}
		}

		if (pos != 0)
		{
			OutValue <<= (8 - pos);
			fputc(OutValue, fOut);
		}

		//////////////////////////////////////////////////////
		//д�����ļ�

		string ConfFileName = string(filename) + ".HuffConf";
		FILE * ConfFile = fopen(ConfFileName.c_str(), "w");

		if (!ConfFile)
		{
			return false;
		}

		//дͳ��ֵ
		string tmp2 = "Count: " + to_string(count);
		tmp2 += " ";
		fputs(tmp2.c_str(), ConfFile);

		//дhuffman����Ϣ

		for (int i = 0; i < 256; ++i)
		{
			if (_infos[i]._count != 0)
			{
				string tmp;
				tmp += _infos[i]._ch;
				tmp += " ";
				tmp += _infos[i]._code;
				tmp += " ";  
				tmp += to_string(_infos[i]._count);
				tmp += " ";
				fputs(tmp.c_str(), ConfFile);

			}
		}

		fflush(fp);
		fflush(fOut);
		fflush(ConfFile);


        //�ر��ļ�
		fclose(fp);
		fclose(fOut);
		fclose(ConfFile);
		
		return true;
	}


	//��ѹ������Ҫ����ļ��Ƿ�Ϊ���ǵĹ涨�ĺ�׺��
	bool UnCompress(const char * filename)
	{
		string FileName(filename);
		auto it=FileName.rfind('.');

		string Extension(FileName.begin()+it, FileName.end());
		string newFile(FileName.begin(), FileName.begin() + it);

		//newFile += ".new";

		if (Extension == ".huffcom")
		{
			//��ȷ��ѹ���ļ�
			//cout << "Ok" << endl;
			
			
			//�����ѹ�ɹ�
			if (_UnCompress(filename, newFile.c_str()))
				return true;
			else
				return false;
		}
		else
		{
			//ȡ����ѹ��
			cout << "����ѹ���ļ���ʽ" << endl;
			return false;
		}
	}

private:

	//����һ��ȷ�����ļ�ִ�н�ѹ������  д���ļ�newfile
	bool _UnCompress(const char *filename,const char * newFile)
	{
		// ���ļ�
		FILE * fIn = fopen(filename, "rb");
		if (fIn == NULL)
			return false;
		FILE * fOut = fopen((string(newFile)+".new").c_str(), "wb");
		if (fIn == NULL)
			return false;
		//�������ļ�
		FILE* fConf = fopen((string(newFile) + ".HuffConf").c_str(), "rb");
		if (fConf == NULL)
			return false;

		//���������ļ� ��Huffman��
		
		//1.���_infos����Ϣ
		for (int i = 0; i < 256; ++i)
		{
			//���
			_infos[i]._ch = i;
			_infos[i]._code = "";
			_infos[i]._count = 0;

		}

		//2.�������ļ�  ��������
		//unsigned char ch=fgetc(fConf);
		char tmp[7];
		long  SumBit;
		fscanf(fConf, "%s %ld", tmp, &SumBit);  //��ȡ�ܵ�λ��
		fseek(fConf, 1, SEEK_CUR);

		
		char * code = new char[(OutputMaxCodeSize()+1)];
		
		memset(code, 0, OutputMaxCodeSize() + 1);
		
		unsigned char ch=0;
		int count = 0;

		
		while (!feof(fConf) && ((char)(ch = fgetc(fConf))!=EOF))
		{
			fseek(fConf, 1, SEEK_CUR);  //�ߵ�һ���ո�
			memset(code, 0, OutputMaxCodeSize() + 1);
			fscanf(fConf, "%s %ld",code, &count);
			fseek(fConf, 1, SEEK_CUR);  //�ߵ�һ���ո�

			_infos[ch]._count = count;
			_infos[ch]._code=string((const char *)code);
		}
		
#ifdef _DEBUG_
		Print();
#endif
		//����ѹ���ļ� ����  �����newFile

		

		/////////////////////////////////////////////////////////////////////////////////////////////
		//����_infos[256]����,��HuffmanTree

		CharInfo invalid;
		invalid._count = 0;
		
		HuffmanTree<CharInfo> ht;
		ht.CreateHuffmanTree(_infos, 256, invalid);
		//ht.CountCode(_infos);

		////////////////////////////////////////////////////////////
		///////////������д����

		ch = 0;
		int pos=0;  //һ���ֽ��е�λ��
		auto root = ht.GetRoot();
		auto node = root;

		ch = fgetc(fIn);

		while (SumBit>0)
		{
			if (node->_left == NULL&&node->_right == NULL)
			{
				//д���ļ�
				fputc((unsigned char)node->_weight._ch, fOut);
				node = root;
				--SumBit;
			}
			else
			{
				bool tmp = ch & ((int)0x80 >> pos);

				//�����true �� ������  false ��Ϊ������
				if (tmp)
				{
					node = node->_right;
				}
				else
				{
					node = node->_left;
				}

				++pos;
				
				if (pos == 8)
				{
					if (!feof(fIn))
					{
						ch = fgetc(fIn);
						pos = 0;
					}
					else
						break;
				}

			}
			
		}

		//�������һ��λ
		if (node->_left == NULL&&node->_right == NULL)
		{
			//д���ļ�
			fputc(node->_weight._ch, fOut);
			node = root;
		}

		fflush(fIn);
		fflush(fOut);
		fflush(fConf);

		//delete[] code;
		fclose(fIn);
		fclose(fOut);
		fclose(fConf);  //���ر��ļ��ᵼ���쳣�˳�
		return true;

	}

	void Print()
	{
		for (int i = 0; i < 256; ++i)
		{
			if (_infos[i]._code != "" && _infos[i]._count != 0)
				cout << _infos[i]._ch << ":" << _infos[i]._code << endl;
		}
	}

	int OutputMaxCodeSize()
	{
		int MaxSize = _infos[0]._code.size();
		for (int i = 1; i < 256; ++i)
		{
			if (MaxSize < _infos[i]._code.size())
			{
				MaxSize = _infos[i]._code.size();
			}
		}

		return MaxSize;
	}



protected:
	CharInfo _infos[256];

};


