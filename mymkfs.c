#include<stdio.h>
#include<time.h>
int main(){
	FILE *fop,*fch; 
	time_t now;
	now=time(NULL);
	
	fch=fopen("myfs","r");
	if(fch!=NULL){
		printf("error : myfs exitsts\n");
		return 0;
	}
	else
		fop=fopen("myfs","wb");
	char boot[2]={0,0}; //boot 블록
	unsigned char inode_ch[64]={128}; 
	//128은 ich[0]=1000 0000(2진수)넣은것 inode_check
	unsigned char data_ch[128]={128}; 
	//128은 dch[0]=1000 0000(2진수)넣은것 data_check
	
	short inode_c=1,data_c=1; 
	//inode 1개,data 1개  inode_count,data_count
	short inode_n=0,data_n=0;
	//inode 0번,data 0번 배열상 첫번째를 의미

	struct inode_st{
		char type;
		short direct_b; //direct block 숫자 0~1023
		struct tm creat_time;
		/***********************
		  사용법 : time_t now;
		  now=time(NULL);
		  creat_time=*(localtime(&now));
		 ************************/		  
		int size;
		short indirect_b; //indirect block 숫자 0~1023
		short douindirect_b; //double indirect block 숫자 0~1023
	}root_inode; 
	//루트 디렉토리의 inode 1번
	
	root_inode.type='d'; //direcotry를 의미 regular은 r
	
	root_inode.creat_time=*(localtime(&now)); //생성 시간
	
	root_inode.size=0; //   /는 폴더 이므로 size가 0
	root_inode.direct_b=0; //0은 배열상 1번째를 의미
	root_inode.indirect_b=-1; //없기 때문에 -1값
	root_inode.douindirect_b=-1; //없기 때문에 -1값
	
	unsigned char root_data[128]={0};
	//root디렉토리의 data블록정보 1번
	*((short *)root_data)=0;
	*((short *)(root_data+2))=0;
	//direcotry 파일의 data blcok 처음 2바이트 두번째 2바이트는
	//. 과 ..을 나타내는데 사용
	//.은 현재 디렉토리 ..은 부모디렉토리
	fwrite(boot,sizeof(boot),1,fop);
	fwrite(inode_ch,sizeof(inode_ch),1,fop);
	fwrite(data_ch,sizeof(data_ch),1,fop);
	fwrite(&inode_c,sizeof(inode_c),1,fop);
	fwrite(&data_c,sizeof(data_c),1,fop);
	fwrite(&inode_n,sizeof(inode_n),1,fop);
	fwrite(&root_inode,sizeof(root_inode),1,fop);
	fwrite(&data_n,sizeof(data_n),1,fop);
	fwrite(root_data,sizeof(root_data),1,fop);


	fclose(fch);
	fclose(fop);
	return 0;
}
