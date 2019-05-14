    
//#include <Windows.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "winsock2.h"
#include <time.h>
#include <queue>
#include <set>
#include <windows.h>
#include<cstdlib>
 
#pragma comment(lib, "ws2_32.lib") 
using namespace std;
 
#define DEFAULT_PAGE_BUF_SIZE 1048576
 
queue<string> hrefUrl;
set<string> visitedUrl;
set<string> visitedImg;
int depth=0;
int g_ImgCnt=1;
 
 
 string shortname = "def", midname;
 	string urlStart = "http://www.baidu.net/";
 	string urlhost = "http://www.baidu.com/";
//解析URL，解析出主机名，资源名
bool ParseURL( const string & url, string & host, string & resource){
	if ( strlen(url.c_str()) > 2000 ) {
		return false;
	}
 
	const char * pos = strstr( url.c_str(), "http://" );
	if( pos==NULL ) pos = url.c_str();
	else pos += strlen("http://");
	if( strstr( pos, "/")==0 )
		return false;
	char pHost[100];
	char pResource[2000];
	sscanf( pos, "%[^/]%s", pHost, pResource );
	host = pHost;
	resource = pResource;
	return true;
}
 
 
inline bool myfindstr(string a, string b) {
	if(a.length() < b.length()) return 0;
	for(int i = 0; i <= a.length() - b.length(); i++) {
		int pd = 1;
		for(int j = i; j < i+b.length(); j++) if(a[j] != b[j-i]) {
			pd =0 ;
			break;
		}
		if(pd) return 1;
	}
	return 0;
}

//使用Get请求，得到响应
bool GetHttpResponse( string url, char * &response, int &bytesRead , int prt){
	if(url[0] != 'h' || url[1] != 't' || url[2] != 't') url = "http:" + url;
	if(url[4] == 's') {
		string tmp;
		for(int i = 0; i < 4; i++) tmp += url[i];
		for(int i = 5; i < url.length(); i++) tmp += url[i];
		url = tmp; 
	}
	if(prt) cout<<"Linking - "<<url<<" ";
//	if(url.back() == 'm' && url[url.length()-2] == 'o') url += '/'; 
	string host, resource;
	if(prt && ParseURL(url+"/", host, resource)) url += "/";
	if(!ParseURL( url, host, resource )){
		cout << "Error: URL Parsing Error"<<endl;//:"<<url<<endl;
//		if(prt) system("pause");
		return false;
	}
	
	//建立socket
	struct hostent * hp= gethostbyname( host.c_str() );
	if( hp==NULL ){
		cout<< "Error: No Such Host"<<endl;//:"<<url<<endl;
//		if(prt) system("pause");
		return false;
	}
 
	SOCKET sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if( sock == -1 || sock == -2 ){
		cout << "错误！不能造火箭。"<<endl;
//		if(prt) system("pause");
		return false;
	}
 
	//建立服务器地址
	SOCKADDR_IN sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons( 80 );
	//char addr[5];
	//memcpy( addr, hp->h_addr, 4 );
	//sa.sin_addr.s_addr = inet_addr(hp->h_addr);
	memcpy( &sa.sin_addr, hp->h_addr, 4 );
 
	//建立连接
	if( 0!= connect( sock, (SOCKADDR*)&sa, sizeof(sa) ) ){
		cout << "错误！无法连接" <<endl;
		closesocket(sock);
//		if(prt) system("pause");
		return false;
	};
 
	//准备发送数据
	string request = "GET " + resource + " HTTP/1.1\r\nHost:" + host + "\r\nConnection:Close\r\n\r\n";
 
	//发送数据
	if( SOCKET_ERROR ==send( sock, request.c_str(), request.size(), 0 ) ){
		cout << "错误！数据发送时错误" <<endl;
		closesocket( sock );
//		if(prt) system("pause");
		return false;
	}
	if(prt) cout <<"Success!"<<endl<<"Read: ";
	
	//接收数据
	int m_nContentLength = DEFAULT_PAGE_BUF_SIZE;
	char *pageBuf = (char *)malloc(m_nContentLength);
    memset(pageBuf, 0, m_nContentLength);
 
    bytesRead = 0;
	int ret = 1;
	int tim = time(0);
    while(ret > 0){
    	cerr<<".";
        ret = recv(sock, pageBuf + bytesRead, m_nContentLength - bytesRead, 0);
        cerr<<".";
        if(ret > 0)
        {
            bytesRead += ret;
        }
 
		if( m_nContentLength - bytesRead<100){
			cout << "\nRealloc memorry"<<endl;
			m_nContentLength *=2;
			pageBuf = (char*)realloc( pageBuf, m_nContentLength);       //重新分配内存
		}
		if(prt) cout << ret <<" ";
//        if(time(0) > tim + 1) {
//        	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY |FOREGROUND_RED|FOREGROUND_GREEN);
//        	cerr<<"Time out"<<endl;
//			 SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY);
//			 break;
//		}
    }
	if(prt) cout <<endl;
 
    pageBuf[bytesRead] = '\0';
	response = pageBuf;
	closesocket( sock );
//	cout<<"成功 
	return true;
	//cout<< response <<endl;
}

inline void dispurl(string& s, string host) {
	if(s[0] == 'h' && s[1] == 't' && s[2] == 't');
	else if(s[0] == '/' && s[1] == '/') {
		s = "http:" + s;
	}
	else{
		if(s[0] != '/') s = host + s;
		else s = urlhost + s;
	}
//	while(s[0] == '.') s.erase(s.begin());
//	if(s[0] != 'h' || s[1] != 't' || s[2] != 't') {
//		if(s[0] != '/') s = "/" + s;
//		s = urlhost + s;
//	}
	if(s[4] == 's') {
		string tmp;
		for(int i = 0; i < 4; i++) tmp += s[i];
		for(int i = 5; i < s.length(); i++) tmp += s[i];
		s = tmp; 
	}
//	if(s.back() != '/') s += "/";
}
 
//提取所有的URL以及图片URL
void HTMLParse ( string & htmlResponse, vector<string> & imgurls, const string & host ){
	cout<<"    Parsing..."<<endl;// - "<<host<<endl;
	//找所有连接，加入queue中
	const char *p= htmlResponse.c_str();
	char *tag="href='";
	const char *pos = strstr( p, tag );
	ofstream ofile("url.txt", ios::app);
	int cnt = 0;
	while( pos ){
		pos +=strlen(tag);
		const char * nextQ = strstr( pos, "'" );
		if( nextQ ){
			char * url = new char[ nextQ-pos+1 ];
			//char url[100]; //固定大小的会发生缓冲区溢出的危险
			sscanf( pos, "%[^']", url);
			string surl = url;  // 转换成string类型，可以自动释放内存
//			cout<<"original link: "<<surl<<endl;
//			if(surl[0] == '.') {
//				surl[0] = '/';
//			}
			
			dispurl(surl, host);
			if( !visitedUrl.count( surl )  ){
				visitedUrl.insert( surl );
				ofile << surl<<endl;
				cout<<"    To - "<<surl<<endl; 
//		if(++cnt > 5) break;  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!JUSTFORTESTING. REMOVEING
				hrefUrl.push( surl );
			}
			pos = strstr(pos, tag );
			delete [] url;  // 释放掉申请的内存
		}
	}
	tag = "href=\"";
	pos = strstr(p, tag);
	while( pos ){
		pos +=strlen(tag);
		const char * nextQ = strstr( pos, "\"" );
		if( nextQ ){
			char * url = new char[ nextQ-pos+1 ];
			sscanf( pos, "%[^\"]", url);
			string surl = url;  // 转换成string类型，可以自动释放内存
//			cout<<"original link: "<<surl<<endl;
			dispurl(surl, host);
			if( !visitedUrl.count( surl )  ){
				visitedUrl.insert( surl );
				ofile << surl<<endl;
				cout<<"    To - "<<surl<<endl; 
				hrefUrl.push( surl );
			}
			pos = strstr(pos, tag );
			delete [] url;  // 释放掉申请的内存
		}
	}
	ofile << endl << endl;
	ofile.close();
 
	tag ="<img ";
	const char* att1= "src=\"";
	const char* att2="lazy-src=\"";
	const char *pos0 = strstr( p, tag );
	while( pos0 ){
		pos0 += strlen( tag );
		const char* pos2 = strstr( pos0, att2 );
		if( !pos2 || pos2 > strstr( pos0, ">") ) {
			pos = strstr( pos0, att1);
			if(!pos) {
				pos0 = strstr(att1, tag );
			continue;
			} else {
				pos = pos + strlen(att1);
			}
		}
		else {
			pos = pos2 + strlen(att2);
		}
 
		const char * nextQ = strstr( pos, "\"");
		if( nextQ ){
			char * url = new char[nextQ-pos+1];
			sscanf( pos, "%[^\"]", url);
//			cout << url<<endl;
			string imgUrl = url;
			if( visitedImg.find( imgUrl ) == visitedImg.end() ){
				visitedImg.insert( imgUrl );
				imgurls.push_back( imgUrl );
			}
			pos0 = strstr(pos0, tag );
			delete [] url;
		}
	}
//	cout << "解析完成"<<endl;
}
 
//把URL转化为文件名
string ToFileName( const string &url ){
	string fileName;
	fileName.resize( url.size());
	int k=0;
	for( int i=0; i<(int)url.size(); i++){
		char ch = url[i];
		if( ch!='\\'&&ch!='/'&&ch!=':'&&ch!='*'&&ch!='?'&&ch!='"'&&ch!='<'&&ch!='>'&&ch!='|')
			fileName[k++]=ch;
	}
	return fileName.substr(0,k) + ".txt";
}
 

//下载图片到img文件夹
void DownLoadImg( vector<string> & imgurls, string url ){
//	url = "http:" +url; 
	cout<<"    Downloading..."<<endl;// - "<<url<<endl; 
	//生成保存该url下图片的文件夹
	string foldname = ToFileName( url );
	foldname = "./img"+shortname+"/"+foldname;
//	foldname = "./img";
	int alcre = 0;
//	if(!CreateDirectory( foldname.c_str(),NULL ));
//		cout << "不能创建目录"<< foldname<<endl;
	char *image;
	int byteRead;
	for( int i=0; i<imgurls.size(); i++){
		//判断是否为图片，bmp，jgp，jpeg，gif 
		dispurl(imgurls[i], url);
		string str = imgurls[i];
		int pos = str.find_last_of(".");
		if( pos == string::npos )
			continue;
		else{
			string ext = str.substr( pos+1, str.size()-pos-1 );
//			if( ext!="bmp"&& ext!="jpg" && ext!="jpeg"&& ext!="gif"&&ext!="png"&&ext!="swf")
//				continue;
		}
		//下载其中的内容
		cout<<"    ";
		if( GetHttpResponse(imgurls[i], image, byteRead, 0)){
//			cout<<" O(∩_∩)O 下载了第"<<g_ImgCnt++<<"张图片"<<endl; 
			
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY |FOREGROUND_GREEN);
			cout<<"O(∩_∩)O Downloaded ("<<g_ImgCnt++<<") - "<<imgurls[i]<<endl; 
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY );
			if ( strlen(image) ==0 ) {
				continue;
			}
			const char *p=image;
			const char * pos = strstr(p,"\r\n\r\n")+strlen("\r\n\r\n");
			int index = imgurls[i].find_last_of("/");
			if( index!=string::npos ){
				if(!alcre) {
					alcre = 1;
					if(!CreateDirectory( foldname.c_str(),NULL ));
				}
				string imgname = imgurls[i].substr( index , imgurls[i].size() );
				ofstream ofile( foldname+imgname, ios::binary );
				if( !ofile.is_open() )
					continue;
//				cout <<g_ImgCnt++<< foldname+imgname<<endl;
				ofile.write( pos, byteRead- (pos-p) );
				ofile.close();
			}
			free(image);
//			return; // ！！！这会导致你至下一张。测试。 
		}else {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY |FOREGROUND_RED);
			cout<<" - "<<imgurls[i]<<endl; 
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
//			system("pause");
		}
	}
//	cout<<"下载完成"<<endl; 
}
 
 
 
//广度遍历
void BFS(  string  url ){
	char * response;
	int bytes;
	// 获取网页的相应，放入response中。
	if(url[0] != 'h' || url[1] != 't' || url[2] != 't') url = "http:" + url;
	if(url[4] == 's') {
		string tmp;
		for(int i = 0; i < 4; i++) tmp += url[i];
		for(int i = 5; i < url.length(); i++) tmp += url[i];
		url = tmp; 
	}
	if(!myfindstr(url, midname)) {
		cout<<"Out of Host"<<endl;
		return; 
	}
//	if(url.back() != '/') url += "/";
	if( !GetHttpResponse( url, response, bytes, 1 ) ){
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY |FOREGROUND_RED);
//cout<<"Hello"<<endl;
//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY |
//FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
//cout<<"World"<<endl;
		cout << "No Response: "<<url<<endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY );
		return;
	}
	string httpResponse=response;
	free( response );
	string filename = ToFileName( url );
	ofstream ofile( "./html/"+filename );
	if( ofile.is_open() ){
		// 保存该网页的文本内容
		ofile << httpResponse << endl;
		ofile.close();
	}
	vector<string> imgurls;
	//解析该网页的所有图片链接，放入imgurls里面
	HTMLParse( httpResponse,  imgurls, url );
	
	//下载所有的图片资源
	DownLoadImg( imgurls, url );
}
 
int main()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY );
	
	cout<<"Input shortname, midname, host, url. The last char musnt't be '/'."<<endl;
	
//	cout<<"例: google google.com http://www.google.com //www.google.com/s?w=\"myjs\""<<endl; 
	cin >> shortname >> midname  >> urlhost >> urlStart;
	//初始化socket，用于tcp网络连接
    WSADATA wsaData;
    if( WSAStartup(MAKEWORD(2,2), &wsaData) != 0 ){
        return 0;
    }
 
	// 创建文件夹，保存图片和网页文本文件
	string tmp = "./img"+shortname;
	CreateDirectory( tmp.c_str(),0);
	CreateDirectory("./html",0);
	//string urlStart = "http://hao.360.cn/meinvdaohang.html";
 
	// 遍历的起始地址
	// string urlStart = "http://www.wmpic.me/tupian";
//	string urlStart = "http://item.taobao.com/item.htm?spm=a230r.1.14.19.sBBNbz&id=36366887850&ns=1#detail";

	
	// 使用广度遍历
	// 提取网页中的超链接放入hrefUrl中，提取图片链接，下载图片。
	BFS( urlStart );
 
	// 访问过的网址保存起来
	visitedUrl.insert( urlStart );
 
	while( hrefUrl.size()!=0 ){
		string url = hrefUrl.front();  // 从队列的最开始取出一个网址
		cout<<"Remaining "<< hrefUrl.size()<<"webs in queue"<<endl; 
//		cout << "队列首"<<url << endl;
		BFS( url );					  // 遍历提取出来的那个网页，找它里面的超链接网页放入hrefUrl，下载它里面的文本，图片
		hrefUrl.pop();                 // 遍历完之后，删除这个网址
//		cout<<"出队"<<endl;
	}
    WSACleanup();
    cout<<"Finish."<<endl;
	system("pause"); 
    return 0;
}
