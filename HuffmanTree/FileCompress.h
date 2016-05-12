
#include<stdlib.h>
#include<string>
#include<iostream>
using namespace std;
typedef unsigned long LongType;
//字符信息，为哈夫曼的T类型 ，需要重载一些函数
struct CharInfo
{
	unsigned char _ch;  //字符
	LongType _count;  //字符在文件出现的次数
	string _code;   //字符的哈夫曼编码

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
		CharInfo tmp(_ch);  //不关心他的值
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

	//压缩
	bool Compress(const char * filename)
	{
		//计数
		FILE * fp = fopen(filename, "r");
		if (!fp)
			return false;
		int ch;
		while ((ch = fgetc(fp)) != EOF)
		{
			++_infos[ch]._count;
		}

		//创建HuffmanTree
		HuffmanTree<CharInfo> hf;
		CharInfo invalid;
		invalid._count = 0;
		hf.CreateHuffmanTree(_infos, 256, invalid);

		//将_code计算出来  需要递归

		hf.CountCode(_infos);
		
#ifdef _DEBUG_
		Print();
#endif
		//读文件 写入文件
		string OutFileName = string(filename) + ".huffcom";

		FILE* fOut = fopen(OutFileName.c_str(), "w");
		fseek(fp, 0, SEEK_SET);  //

		int pos = 0;  //代表位置
		unsigned char OutValue = 0;  //写入的值
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
		//写配置文件

		string ConfFileName = string(filename) + ".HuffConf";
		FILE * ConfFile = fopen(ConfFileName.c_str(), "w");

		if (!ConfFile)
		{
			return false;
		}

		//写统计值
		string tmp2 = "Count: " + to_string(count);
		tmp2 += " ";
		fputs(tmp2.c_str(), ConfFile);

		//写huffman树信息

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


        //关闭文件
		fclose(fp);
		fclose(fOut);
		fclose(ConfFile);
		
		return true;
	}


	//解压缩，需要检查文件是否为我们的规定的后缀名
	bool UnCompress(const char * filename)
	{
		string FileName(filename);
		auto it=FileName.rfind('.');

		string Extension(FileName.begin()+it, FileName.end());
		string newFile(FileName.begin(), FileName.begin() + it);

		//newFile += ".new";

		if (Extension == ".huffcom")
		{
			//正确的压缩文件
			//cout << "Ok" << endl;
			
			
			//如果解压成功
			if (_UnCompress(filename, newFile.c_str()))
				return true;
			else
				return false;
		}
		else
		{
			//取消解压缩
			cout << "不是压缩文件格式" << endl;
			return false;
		}
	}

private:

	//对于一个确定的文件执行解压缩操作  写入文件newfile
	bool _UnCompress(const char *filename,const char * newFile)
	{
		// 打开文件
		FILE * fIn = fopen(filename, "rb");
		if (fIn == NULL)
			return false;
		FILE * fOut = fopen((string(newFile)+".new").c_str(), "wb");
		if (fIn == NULL)
			return false;
		//打开配置文件
		FILE* fConf = fopen((string(newFile) + ".HuffConf").c_str(), "rb");
		if (fConf == NULL)
			return false;

		//根据配置文件 建Huffman树
		
		//1.清空_infos的信息
		for (int i = 0; i < 256; ++i)
		{
			//清空
			_infos[i]._ch = i;
			_infos[i]._code = "";
			_infos[i]._count = 0;

		}

		//2.打开配置文件  读出数据
		//unsigned char ch=fgetc(fConf);
		char tmp[7];
		long  SumBit;
		fscanf(fConf, "%s %ld", tmp, &SumBit);  //读取总的位数
		fseek(fConf, 1, SEEK_CUR);

		
		char * code = new char[(OutputMaxCodeSize()+1)];
		
		memset(code, 0, OutputMaxCodeSize() + 1);
		
		unsigned char ch=0;
		int count = 0;

		
		while (!feof(fConf) && ((char)(ch = fgetc(fConf))!=EOF))
		{
			fseek(fConf, 1, SEEK_CUR);  //走掉一个空格
			memset(code, 0, OutputMaxCodeSize() + 1);
			fscanf(fConf, "%s %ld",code, &count);
			fseek(fConf, 1, SEEK_CUR);  //走掉一个空格

			_infos[ch]._count = count;
			_infos[ch]._code=string((const char *)code);
		}
		
#ifdef _DEBUG_
		Print();
#endif
		//遍历压缩文件 译码  输出到newFile

		

		/////////////////////////////////////////////////////////////////////////////////////////////
		//利用_infos[256]解码,建HuffmanTree

		CharInfo invalid;
		invalid._count = 0;
		
		HuffmanTree<CharInfo> ht;
		ht.CreateHuffmanTree(_infos, 256, invalid);
		//ht.CountCode(_infos);

		////////////////////////////////////////////////////////////
		///////////利用树写数据

		ch = 0;
		int pos=0;  //一个字节中的位置
		auto root = ht.GetRoot();
		auto node = root;

		ch = fgetc(fIn);

		while (SumBit>0)
		{
			if (node->_left == NULL&&node->_right == NULL)
			{
				//写入文件
				fputc((unsigned char)node->_weight._ch, fOut);
				node = root;
				--SumBit;
			}
			else
			{
				bool tmp = ch & ((int)0x80 >> pos);

				//如果是true 则 右子树  false 则为左子树
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

		//补上最后一个位
		if (node->_left == NULL&&node->_right == NULL)
		{
			//写入文件
			fputc(node->_weight._ch, fOut);
			node = root;
		}

		fflush(fIn);
		fflush(fOut);
		fflush(fConf);

		//delete[] code;
		fclose(fIn);
		fclose(fOut);
		fclose(fConf);  //不关闭文件会导致异常退出
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


