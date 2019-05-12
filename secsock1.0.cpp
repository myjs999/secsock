//#include <Windows.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "winsock2.h"
#include <time.h>
#include <queue>
#include <set>
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
//����URL������������������Դ��
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
	for(int i = 0; i < a.length() - b.length(); i++) {
		int pd = 1;
		for(int j = i; j < i+b.length(); j++) if(a[j] != b[j-i]) {
			pd =0 ;
			break;
		}
		if(pd) return 1;
	}
	return 0;
}
//ʹ��Get���󣬵õ���Ӧ
bool GetHttpResponse( string url, char * &response, int &bytesRead ){
	
	string host, resource;
	if(!ParseURL( url, host, resource )){
		cout << "!!!!!!!!!!���ܽ���URL:"<<url<<endl;
		return false;
	}
	
	//����socket
	struct hostent * hp= gethostbyname( host.c_str() );
	if( hp==NULL ){
		cout<< "!!!!!!!!!!�Ҳ�������λ��:"<<url<<endl;
		return false;
	}
 
	SOCKET sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if( sock == -1 || sock == -2 ){
		cout << "����������"<<endl;
		return false;
	}
 
	//������������ַ
	SOCKADDR_IN sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons( 80 );
	//char addr[5];
	//memcpy( addr, hp->h_addr, 4 );
	//sa.sin_addr.s_addr = inet_addr(hp->h_addr);
	memcpy( &sa.sin_addr, hp->h_addr, 4 );
 
	//��������
	if( 0!= connect( sock, (SOCKADDR*)&sa, sizeof(sa) ) ){
		cout << "!!!!!!!!!!�޷����ӵ�:"<< url <<endl;
		closesocket(sock);
		return false;
	};
 
	//׼����������
	string request = "GET " + resource + " HTTP/1.1\r\nHost:" + host + "\r\nConnection:Close\r\n\r\n";
 
	//��������
	if( SOCKET_ERROR ==send( sock, request.c_str(), request.size(), 0 ) ){
		cout << "���ݷ���ʱ����" <<endl;
		closesocket( sock );
		return false;
	}
	cout <<"������"<<url<<endl;
	
	//��������
	int m_nContentLength = DEFAULT_PAGE_BUF_SIZE;
	char *pageBuf = (char *)malloc(m_nContentLength);
    memset(pageBuf, 0, m_nContentLength);
 
    bytesRead = 0;
	int ret = 1;
    while(ret > 0){
        ret = recv(sock, pageBuf + bytesRead, m_nContentLength - bytesRead, 0);
        
        if(ret > 0)
        {
            bytesRead += ret;
        }
 
		if( m_nContentLength - bytesRead<100){
			cout << "\nRealloc memorry"<<endl;
			m_nContentLength *=2;
			pageBuf = (char*)realloc( pageBuf, m_nContentLength);       //���·����ڴ�
		}
		cout << ret <<" ";
    }
	cout <<endl;
 
    pageBuf[bytesRead] = '\0';
	response = pageBuf;
	closesocket( sock );
//	cout<<"�ɹ� 
	return true;
	//cout<< response <<endl;
}
 
//��ȡ���е�URL�Լ�ͼƬURL
void HTMLParse ( string & htmlResponse, vector<string> & imgurls, const string & host ){
//	cout<<"������ʼ"<<endl;
	//���������ӣ�����queue��
	const char *p= htmlResponse.c_str();
	char *tag="href=\"";
	const char *pos = strstr( p, tag );
	ofstream ofile("url.txt", ios::app);
	while( pos ){
		pos +=strlen(tag);
		const char * nextQ = strstr( pos, "\"" );
		if( nextQ ){
			char * url = new char[ nextQ-pos+1 ];
			//char url[100]; //�̶���С�Ļᷢ�������������Σ��
			sscanf( pos, "%[^\"]", url);
			string surl = url;  // ת����string���ͣ������Զ��ͷ��ڴ�
			if(surl[0] == '/') surl = host + surl;
			if( visitedUrl.find( surl ) == visitedUrl.end() ){
				visitedUrl.insert( surl );
				ofile << surl<<endl;
				hrefUrl.push( surl );
			}
			pos = strstr(pos, tag );
			delete [] url;  // �ͷŵ�������ڴ�
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
//	cout << "��������"<<endl;
}
 
//��URLת��Ϊ�ļ���
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
 

//����ͼƬ��img�ļ���
void DownLoadImg( vector<string> & imgurls, string url ){
//	url = "http:" +url; 
	cout<<"��������ͼƬ��"<<url<<endl; 
	//���ɱ����url��ͼƬ���ļ���
	string foldname = ToFileName( url );
	foldname = "./img"+shortname+"/"+foldname;
//	foldname = "./img";
	int alcre = 0;
//	if(!CreateDirectory( foldname.c_str(),NULL ));
//		cout << "���ܴ���Ŀ¼"<< foldname<<endl;
	char *image;
	int byteRead;
	for( int i=0; i<imgurls.size(); i++){
		//�ж��Ƿ�ΪͼƬ��bmp��jgp��jpeg��gif 
		string str = imgurls[i];
		int pos = str.find_last_of(".");
		if( pos == string::npos )
			continue;
		else{
			string ext = str.substr( pos+1, str.size()-pos-1 );
			if( ext!="bmp"&& ext!="jpg" && ext!="jpeg"&& ext!="gif"&&ext!="png")
				continue;
		}
		//�������е�����
		if( GetHttpResponse(imgurls[i], image, byteRead)){
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
				cout <<g_ImgCnt++<< foldname+imgname<<endl;
				ofile.write( pos, byteRead- (pos-p) );
				ofile.close();
			}
			free(image);
		}
	}
}
 
 
 
//��ȱ���
void BFS(  string  url ){
	char * response;
	int bytes;
	// ��ȡ��ҳ����Ӧ������response�С�
	if(url[0] != 'h' || url[1] != 't' || url[2] != 't') url = "http:" + url;
	if(!myfindstr(url, midname)) {
		cout<<"���뿪��վ"<<endl;
		return; 
	}
	if( !GetHttpResponse( url, response, bytes ) ){
		cout << "��ҳ����Ӧ"<<endl;
		return;
	}
	string httpResponse=response;
	free( response );
	string filename = ToFileName( url );
	ofstream ofile( "./html/"+filename );
	if( ofile.is_open() ){
		// �������ҳ���ı�����
		ofile << httpResponse << endl;
		ofile.close();
	}
	vector<string> imgurls;
	//��������ҳ������ͼƬ���ӣ�����imgurls����
	HTMLParse( httpResponse,  imgurls, url );
	
	//�������е�ͼƬ��Դ
	DownLoadImg( imgurls, url );
}
 
int main()
{
	cout<<"����shortname��midname��url"<<endl;
	cin >> shortname >> midname >> urlStart ;
	//��ʼ��socket������tcp��������
    WSADATA wsaData;
    if( WSAStartup(MAKEWORD(2,2), &wsaData) != 0 ){
        return 0;
    }
 
	// �����ļ��У�����ͼƬ����ҳ�ı��ļ�
	string tmp = "./img"+shortname;
	CreateDirectory( tmp.c_str(),0);
	CreateDirectory("./html",0);
	//string urlStart = "http://hao.360.cn/meinvdaohang.html";
 
	// ��������ʼ��ַ
	// string urlStart = "http://www.wmpic.me/tupian";
//	string urlStart = "http://item.taobao.com/item.htm?spm=a230r.1.14.19.sBBNbz&id=36366887850&ns=1#detail";

	
	// ʹ�ù�ȱ���
	// ��ȡ��ҳ�еĳ����ӷ���hrefUrl�У���ȡͼƬ���ӣ�����ͼƬ��
	BFS( urlStart );
 
	// ���ʹ�����ַ��������
	visitedUrl.insert( urlStart );
 
	while( hrefUrl.size()!=0 ){
		string url = hrefUrl.front();  // �Ӷ��е��ʼȡ��һ����ַ
		cout << "������"<<url << endl;
		BFS( url );					  // ������ȡ�������Ǹ���ҳ����������ĳ�������ҳ����hrefUrl��������������ı���ͼƬ
		hrefUrl.pop();                 // ������֮��ɾ�������ַ
//		cout<<"����"<<endl;
	}
    WSACleanup();
    cout<<"��ȡ���"<<endl;
	system("pause"); 
    return 0;
}
