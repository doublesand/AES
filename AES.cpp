#include "AES_tables.h"
#include <string.h>

unsigned char w[44][4];           //10������Կ

unsigned char SBox_Chg(char sin,int bits) {  //���ַ�sin�����ֽ��滻 
//S�б任
	int i,j,k=0;
	char temp,out[8];
	for(j=7; j>=0; j--) { //����к��к�
		temp=sin&(1<<j);
		if(0 == temp) {
			out[k]=0;
			k++;
		} else {
			out[k]=1;
			k++;
		}
	}
	i=out[0]*8+out[1]*4+out[2]*2+out[3];  //�к�
	j=out[4]*8+out[5]*4+out[6]*2+out[7];  //�к�
	return (S_box[i][j]);   //���صõ���S�е�ֵ
}

unsigned char SBox_1_Chg(char sin,int bits) {  //�ͷ���ֵ��S���滻��һ�� 
//S����任
	int i,j,k=0;
	char temp,out[8];
	for(j=7; j>=0; j--) {
		temp=sin&(1<<j);
		if(0 == temp) {
			out[k]=0;
			k++;
		} else {
			out[k]=1;
			k++;
		}
	}
	i=out[0]*8+out[1]*4+out[2]*2+out[3];//�к�
	j=out[4]*8+out[5]*4+out[6]*2+out[7];//�к�
	return (SBox_1[i][j]);      

}

void Getkey(unsigned char key[16]) {  
	//10������Կ����
	static unsigned char temp[4],t[4];
	int i,j,k;
	for(i=0; i<4; i++) {       
		memcpy(w[i],&key[i*4],4);     //���������Կ���д洢��ÿ��32bits��0��4����ԭʼ��Կ 
	}
	for(i=4; i<44; i++) {               
		memcpy(temp,w[i-1],4);          //w[j-1] 
		if(i%4==0) {                    //if(j-1)%4==0,���б任 
			memcpy(t,temp,1);           //3��memcpy��ʵ�ǽ���ѭ������һλ 
			memcpy(temp,&temp[1],3);
			memcpy(&temp[3],t,1);
			for(j = 0,k=(i/4)*4; j<4; j++,k++) {          //����S�б任���RC������� 
				temp[j]=SBox_Chg(temp[j],8)^RC[k-4];      //ÿ��ѭ����RC�������һ���ֽڣ�ѭ���ĴΣ�����g(w[j-1]) 
			}
			for(j=0; j<4; j++) {                          //w[j] = g(w[j-1]) ^ w[i-4] 
				w[i][j]=w[i-4][j]^temp[j];
			}
		} else
			for(j=0; j<4; j++) {                         //w[j] = w[j-4] ^ w[j-1]
				w[i][j]=w[i-4][j]^temp[j];
			}
	}
}

void rows_mov(unsigned char *sin) {  //����λ����
	static unsigned char temp[4];
	memcpy(temp,&sin[4],1);     //��һ�в��䣬�ڶ���ѭ������һλ
	memcpy(&sin[4],&sin[5],3);
	memcpy(&sin[7],temp,1);

	memcpy(temp,&sin[8],2);      //������ѭ��������λ
	memcpy(&sin[8],&sin[10],2);
	memcpy(&sin[10],temp,2);

	memcpy(temp,&sin[12],3);     //������ѭ��������λ
	memcpy(&sin[12],&sin[15],1);
	memcpy(&sin[13],temp,3);
}

void rows_mov_1(unsigned char *sin) {  //����λ����������
	static unsigned char temp[4];
	memcpy(temp,&sin[7],1);
	memcpy(&sin[5],&sin[4],3);
	memcpy(&sin[4],temp,1);       //ѭ������1λ��ʵ��Ϊ�ڴ������໥����

	memcpy(temp,&sin[10],2);
	memcpy(&sin[10],&sin[8],2);
	memcpy(&sin[8],temp,2);      //ѭ������2λ��ʵ��Ϊ�ڴ������໥����

	memcpy(temp,&sin[12],1);
	memcpy(&sin[12],&sin[13],3);
	memcpy(&sin[15],temp,1);      //ѭ������3λ��ʵ��Ϊ�ڴ������໥����
}

unsigned char GF2mul(unsigned char a, unsigned char b) { 
//������GF(2^8)�ϵĳ˷�
	unsigned char bw[4];
	unsigned char res=0;
	int i;
	bw[0] = b;
	for(i=1; i<4; i++) {       //ѭ�����Σ��õ���2��4��8���ֵ
		bw[i] = bw[i-1]<<1;    //ԭ��ֵ��2
		if(bw[i-1]&0x80) {     //�ж�ԭ��ֵ�Ƿ�С��0x80������ԭ�� 
			bw[i]^=0x1b;       //����Ϊ1����ȥ0x1b doublesand 
		}
	}
	for(i=0; i<4; i++) {
		if((a>>i)&0x01) {    //������a��ֵ��ʾΪ1��2��4��8���������
			res ^= bw[i];
		}
	}
	return res;
}

void columnsmix(unsigned char *sin) {
//�л�ϱ任
	unsigned char t[4][4]= {0};
	int i,j,k;
	for(i=0; i<4; i++)
		for(k=0; k<4; k++)
			for(j=0; j<4; j++)
				t[i][k]^=GF2mul(L_mix[i][j],sin[j*4+k]);  //�������㣺c[i][k] += a[i][j] * b[j][k]
	memcpy(sin,t,16);
}


void columnsmix_1(unsigned char *sin) {
//���л�ϱ任
	unsigned char t[4][4]= {0};
	int i,j,k;
	for(i=0; i<4; i++)
		for(k=0; k<4; k++)
			for(j=0; j<4; j++)
				t[i][k]^=GF2mul(L_mix_1[i][j],sin[j*4+k]);
	memcpy(sin,t,16);
}

void addroundkey_start(unsigned char *sin,unsigned char (*p)[4]) {  //sin[j][i] = sin[i][j]^p[i][j]
//���һ������Կ�Ӳ�������˳����� 
	unsigned char out[16];
	int i,j,k=0;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++) {
			out[j*4+i]=sin[k]^p[i][j];  
			k++;
		}
	memcpy(sin,out,16);
}


void addroundkey(unsigned char *sin,unsigned char (*p)[4]) {
//����Կ�Ӳ�������Ӧ��Ϊ����������ڵ�һ������Կ֮��sin�߼����ǰ������еģ��� sin[j][i] = sin[j][i]^p[i][j] 
	unsigned char out[16];
	int i,j,k=0;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++) {
			out[j*4+i]=sin[j*4+i]^p[i][j];
			k++;
		}
	memcpy(sin,out,16);
}

void AES_Cry(unsigned char *sin) {   //sin����Ϣ����˼ 
	int i,j,k=0;

	addroundkey_start(sin,&w[0]);   //��һ����Կ�Ӳ��� 
	for(i=1; i<10; i++) {            //S���û� 
		for(j=0; j<16; j++)
			sin[j]=SBox_Chg(sin[j],8);
		rows_mov(sin);                   //����λ 
		columnsmix(sin);                 //�л��� 
		addroundkey(sin,&w[i*4]);        //����Կ�� 

	}
	for(j=0; j<16; j++)                //��ʮ���ּ���  
		sin[j]=SBox_Chg(sin[j],8);      //S���û� 
	rows_mov(sin);                      //����λ 
	addroundkey(sin,&w[i*4]);           //����Կ�� 
}

void AES_Dec(unsigned char *sin) {
	int i,j,k=0;                     

	addroundkey(sin,&w[40]);           //����Կ�� 
	for(i=9; i>0; i--) {              
		rows_mov_1(sin);                    //��������λ  
		for(j=0; j<16; j++)
			sin[j]=SBox_1_Chg(sin[j],8);    //�����ֽ��滻 
		addroundkey(sin,&w[i*4]);           //����Կ�� 
		columnsmix_1(sin);                  //�����л��� 
	}
	rows_mov_1(sin);                        //��������λ 
	for(j=0; j<16; j++)                    //�����ֽ��滻 
		sin[j]=SBox_1_Chg(sin[j],8);        
	addroundkey(sin,&w[i*4]);               //����Կ�� 
}

int main() {
	int i,j;

	unsigned char messages[17]= {0}; //����16���ַ��Զ���0 
	unsigned char Mykey[17]= {0};    
	unsigned char Yourkey[17]= {0};  
	printf("��������:(128bit)\n");
	gets((char*)messages);  //����messages��ʱ����char�ͣ���Ӱ�����ļ��� 

	printf("������Կ:(128bit)\n");
	gets((char*)Mykey);

	Getkey(Mykey);
	AES_Cry(messages);
	printf("\n\n���ܺ�");
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			printf("%x ",messages[j*4+i]);              
	printf("\n������Կ:(128bit)\n");
	gets((char*)Yourkey);
	Getkey(Yourkey);
	AES_Dec(messages);
	printf("\n���ܺ�");
	for(i=0; i<4; i++)   //���ܺ���ַ�������˳��ģ�ʵ���ǰ��д洢��4*4�ַ����� 
		for(j=0; j<4; j++)
			printf("%c ",messages[j*4+i]);
	printf("\n");
}
