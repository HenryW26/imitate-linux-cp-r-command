#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <utime.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <fcntl.h>
#include<string.h>
#include <stdlib.h>

int FileToFile(char* source, char* dest, struct stat* pstat);
int DirToDir(char* source, char* dest);

int main(int argc, char* argv[])
{
	struct stat sourcestat;
	struct stat deststat;
	char destbuf[500];
	int ret = 0;

	if (argc != 3)
	{
		printf(" ERROR:Invalid arguments\n");
		return -1;
	}
	else
	{
		printf("source file name is %s\n", argv[1]);
		printf("destination file name is %s\n", argv[2]);
		if (lstat(argv[1], &sourcestat) == -1)
		{
			printf("ERROR:get argv[1] stat error\n");
			return -1;
		}
		if (S_ISDIR(sourcestat.st_mode))
		{
			if (access(argv[2], F_OK) != 0)
			{
				ret = mkdir(argv[2], sourcestat.st_mode);
				printf("mkdir the ret is %d\n", ret);
			}
			else
			{
				if (lstat(argv[2], &deststat) == -1)
				{
					printf("ERROR:get argv[1] stat error\n");
					return -1;
				}
				if (!S_ISDIR(deststat.st_mode))
				{
					printf("ERROR:the dest is exist file\n");
					return -1;
				}
				else
				{
					int len;
					char* pSrc;

					len = strlen(argv[1]);
					pSrc = argv[1] + (len - 1); //ָ�����һ���ַ�
					/*���ҳ�Դ�ļ����ļ���*/
					while (pSrc >= argv[1] && *pSrc != '/')
					{
						pSrc--;
					}
					pSrc++;//ָ��Դ�ļ���
					printf("the dest file is %s\n", pSrc);
					memset(destbuf, 0, 500);
					sprintf(destbuf, "%s/%s", argv[2], pSrc);
					printf("the dest path is %s\n", destbuf);

					if (access(destbuf, F_OK) != 0)
					{
						ret = mkdir(destbuf, 0755);
						printf("mkdir the ret is %d\n", ret);
					}
					ret = DirToDir(argv[1], destbuf);
					return ret;
				}
			}
			ret = DirToDir(argv[1], argv[2]);
			return ret;
		}
		else
		{
			if (access(argv[2], F_OK) == 0)
			{
				printf("dest file exist\n");
				if (lstat(argv[2], &deststat) == -1)
				{
					printf("ERROR:get argv[1] stat error\n");
					return -1;
				}
				if (S_ISDIR(deststat.st_mode))
				{
					int len;
					char* pSrc;

					len = strlen(argv[1]);
					pSrc = argv[1] + (len - 1); //ָ�����һ���ַ�
					/*���ҳ�Դ�ļ����ļ���*/
					while (pSrc >= argv[1] && *pSrc != '/')
					{
						pSrc--;
					}
					pSrc++;//ָ��Դ�ļ���
					printf("the dest file is %s\n", pSrc);
					memset(destbuf, 0, 500);
					sprintf(destbuf, "%s/%s", argv[2], pSrc);
					printf("the dest path is %s\n", destbuf);
					ret = FileToFile(argv[1], destbuf, &sourcestat);
					return ret;
				}
				else
				{
					ret = FileToFile(argv[1], argv[2], &sourcestat);
					return ret;
				}
			}
			else
			{
				int len;
				char* pSrc;

				ret = mkdir(argv[2], 0755);
				printf("mkdir the ret is %d\n", ret);

				len = strlen(argv[1]);
				pSrc = argv[1] + (len - 1); //ָ�����һ���ַ�
				/*���ҳ�Դ�ļ����ļ���*/
				while (pSrc >= argv[1] && *pSrc != '/')
				{
					pSrc--;
				}
				pSrc++;//ָ��Դ�ļ���
				printf("the dest file is %s\n", pSrc);
				memset(destbuf, 0, 500);
				sprintf(destbuf, "%s/%s", argv[2], pSrc);
				printf("the dest path is %s\n", destbuf);
				ret = FileToFile(argv[1], destbuf, &sourcestat);
				return ret;
			}
		}
	}
}

int FileToFile(char* source, char* dest, struct stat* pstat)
{
	int sourcefd = 0;
	int destfd = 0;
	char buf[4096];
	int readcount = 4096;
	int writecount = 0;
	int datacount = 0;
	sourcefd = open(source, O_RDONLY);
	datacount = (int)pstat->st_size;
	destfd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, pstat->st_mode);

	while (datacount) {
	if (datacount < readcount) {
		readcount = datacount;
	}
	if ((writecount = read(sourcefd, buf, readcount)) == readcount)
	{
		write(destfd, buf, writecount);
	}
	else {
		printf("read error\n");
		return -1;
	}
	datacount -= writecount;
	}
	return 0;


}

int DirToDir(char* source, char* dest)
{
	DIR* currentdir = NULL;
	struct dirent* currentdp = NULL;
	struct stat currentstat;
	char sourcebuf[500];
	char destbuf[500];
	int ret = 0;
	currentdir=opendir(source);
	if (currentdir == NULL)
	{
		perror("opendir");
		exit(-1);
	}
	while ((currentdp = readdir(currentdir)) != NULL) {
		memset(&currentstat, 0, sizeof(currentstat));
		if(currentdp->d_name[0]!= '.')
		{
		sprintf(sourcebuf, "%s/%s", source, currentdp->d_name);
		ret = stat(sourcebuf, &currentstat);
		if (ret < 0)
		{
			perror("stat");
			exit(-1);
		}
		if (S_ISDIR(currentstat.st_mode))
		{
			memset(destbuf, 0, 500);
			sprintf(destbuf, "%s/%s", dest, currentdp->d_name);
			if (access(destbuf, F_OK) != 0) {
				mkdir(destbuf, currentstat.st_mode);
			}
			DirToDir(sourcebuf, destbuf);
		}
		else {
			memset(destbuf, 0, 500);
			sprintf(destbuf, "%s/%s", dest, currentdp->d_name);
			FileToFile(sourcebuf, destbuf, &currentstat);
		}
		}
	}
}
