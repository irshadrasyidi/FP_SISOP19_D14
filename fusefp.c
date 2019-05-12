// gcc -Wall `pkg-config fuse --cflags` fusefp.c -o fusefp.out `pkg-config fuse --libs`

// d_type 4 = grup
// d_type 8 = file
#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>

static const char *dirpath = "/home/irshadrasyidi";

void listDir(char* path){
	DIR* dir;
	struct dirent *ent;
	if((dir=opendir(path)) != NULL){
		while (( ent = readdir(dir)) != NULL){
			if(ent->d_type == DT_DIR && strcmp(ent->d_name, ".") != 0  && strcmp(ent->d_name, "..") != 0){
				printf("%s\n", ent->d_name);
				listDir(ent->d_name);
			}
		}
		closedir(dir);
	}
}

void listDirCp(const char* path){
	DIR* dir;
	struct dirent *ent;
	if((dir=opendir(path)) != NULL){
		while (( ent = readdir(dir)) != NULL){
			if(ent->d_type == DT_DIR && strcmp(ent->d_name, ".") != 0  && strcmp(ent->d_name, "..") != 0){
				// printf("%s\n", ent->d_name);
				char temp1[500];
				memset(&temp1, 0, sizeof(temp1));
				char tempa[500];
				memset(&tempa, 0, sizeof(tempa));
				
				strcat(temp1, path);
				strcat(temp1, "/");
				
				strcat(tempa, ent->d_name);
				
				strcat(temp1, tempa);
				
				printf("%s\n", temp1);
				
				listDirCp(temp1);
			}
			else if(ent->d_type == 8 && strcmp(ent->d_name, ".") != 0  && strcmp(ent->d_name, "..") != 0){
				// printf("%s\n", ent->d_name);
				char *end;
				char cekmp3[250];
				memset(&cekmp3, 0, sizeof(cekmp3));

				end = ent->d_name + strlen(ent->d_name);
				while(end > ent->d_name && *end != '.'){
					--end;
				}
				strcat(cekmp3, end);

				if(end > ent->d_name && (strcmp(cekmp3, ".mp3") == 0)){
					printf("COPYYY!!\n\n");
					printf("%s\n", ent->d_name);
					// copy
					const char *cmd;
					char koman[500], dst[500], src[500];
					memset(&koman, 0, sizeof(koman));
					memset(&dst, 0, sizeof(dst));
					memset(&src, 0, sizeof(src));

					sprintf(dst, "/home/irshadrasyidi/%s", ent->d_name);

					sprintf(src, "%s/%s", path, ent->d_name);

					sprintf(koman, "cp %s %s", src, dst);

					cmd = koman;
					system(cmd);
				}
				else{
					continue;
				}
			}
		}
		closedir(dir);
	}
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = lstat(fpath, stbuf);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
  char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;
	
	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {

		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;

		char *end;
		char cekmp3[250];
		memset(&cekmp3, 0, sizeof(cekmp3));

		end = de->d_name + strlen(de->d_name);
		while(end > de->d_name && *end != '.'){
			--end;
		}
		strcat(cekmp3, end);
		
		if(de->d_type == 8){
			if(end > de->d_name && (strcmp(cekmp3, ".mp3") == 0)){
				res = (filler(buf, de->d_name, &st, 0));
					if(res!=0) break;
			}
			else{
				continue;
			}
		}
		else if(de->d_type == 4){
			continue;
		}
		
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
  char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;
  int fd = 0 ;

	(void) fi;
	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
};

int main(int argc, char *argv[])
{
	// char * tes;
	// char hehe[250];
	// memset(&hehe, 0, sizeof(hehe));
	// strcat(hehe, "hehe");
	// tes = hehe;
	// printf("%s\n", tes);

	listDirCp(dirpath);

	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
