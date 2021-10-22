#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>
#include <iostream>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stddef.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdarg.h>

#include "utils/common.hpp"

int (*real_open)(const char* pathname, int flags, ...) = NULL;
ssize_t (*real_read)(int fd, void* buffer, size_t count) = NULL;
ssize_t (*real_write)(int fd, const void* buffer, size_t count) = NULL;
int (*real_close)(int fd) = NULL;
int (*real_access)(const char *pathname, int mode) = NULL;
int (*real_stat)(const char *__restrict pathname, struct stat *__restrict statbuf) = NULL;
int (*real_fstat)(int fd, struct stat *statbuf) = NULL;

std::unordered_map<int, std::pair<void*, long int>> files;
circular_buffer buf_requests; 
int* buf_response;
int i_resp;
sem_t* sem_requests;
sem_t* sem_new_msgs;
sem_t* sem_response;

std::set<int> capio_files_descriptors; //TODO: with unordered_set FORTRAN produces a arithmetic exception
std::unordered_set<std::string> capio_files_paths;


sem_t* get_sem_requests() {
	return sem_open("sem_requests", 0);
}


/*
 * This function must be called only once
 *
 */

void mtrace_init(void) {
	real_open = (int (*)(const char*, int, ...)) dlsym(RTLD_NEXT, "open");
	if (NULL == real_open) {
		fprintf(stderr, "Error in `dlsym open`: %s\n", dlerror());
		return;	
	}
	real_read = (ssize_t (*)(int, void*, size_t)) dlsym(RTLD_NEXT, "read");
	if (NULL == real_read) {
		fprintf(stderr, "Error in `dlsym read`: %s\n", dlerror());
		return;	
	}
	real_write = (ssize_t (*)(int, const void*, size_t)) dlsym(RTLD_NEXT, "write");
	if (NULL == real_write) {
		fprintf(stderr, "Error in `dlsym write`: %s\n", dlerror());
		return;	
	}
	real_close = (int (*)(int)) dlsym(RTLD_NEXT, "close");
	if (NULL == real_close) {	
		fprintf(stderr, "Error in `dlsym close`: %s\n", dlerror());
		return;
	}
	real_access = (int (*)(const char*, int)) dlsym(RTLD_NEXT, "access");
	if (NULL == real_access) {	
		fprintf(stderr, "Error in `dlsym access`: %s\n", dlerror());
		exit(1);
		return;
	}
	real_stat = (int (*)(const char *__restrict, struct stat *__restrict)) dlsym(RTLD_NEXT, "stat");
	if (NULL == real_stat) {	
		fprintf(stderr, "Error in `dlsym stat`: %s\n", dlerror());
		exit(1);
		return;
	}
	real_fstat = (int (*)(int, struct stat*)) dlsym(RTLD_NEXT, "fstat");
	if (NULL == real_fstat) {	
		fprintf(stderr, "Error in `dlsym fstat`: %s\n", dlerror());
		exit(1);
		return;
	}
	buf_requests = get_circular_buffer();
	sem_requests = get_sem_requests();
	sem_new_msgs = sem_open("sem_new_msgs", O_RDWR);
	sem_response = sem_open(("sem_response" + std::to_string(getpid())).c_str(),  O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
	buf_response = (int*) create_shm("buf_response" + std::to_string(getpid()), 4096);
	i_resp = 0;

}

int add_open_request(const char* pathname) {
	int fd;
	std::cout << "open request" << std::endl;
	sem_wait(sem_requests);
	std::string str ("open " + std::to_string(getpid()) + " " + std::string(pathname));
	const char* c_str = str.c_str();
	memcpy(buf_requests.buf + *buf_requests.i, c_str, strlen(c_str) + 1);
	std::cout << "open before response" << std::endl;
	char tmp_str[1024];
	printf("c_str %s, len c_str %i\n", c_str, strlen(c_str));
	sprintf(tmp_str, "%s", (char*) buf_requests.buf + *buf_requests.i);
	printf("add open msg sent: %s\n", tmp_str);

	//for (int i = 0; i < strlen(c_str) + 1; ++i) {
	//	std::cout << ((char*)buf_requests.buf)[i];
	//}
	std::cout << "before wait sem response" << std::endl;

	*buf_requests.i = *buf_requests.i + strlen(c_str) + 1;
	std::cout << "*buf_requests.i == " << *buf_requests.i << std::endl;
	sem_post(sem_requests);
	sem_post(sem_new_msgs);

	//wait for response
		
	sem_wait(sem_response);
	std::cout << "Open after response" << std::endl;
	fd = buf_response[i_resp];
	++i_resp;
	return fd; 
}

int add_close_request(int fd) {
	std::cout << "close request" << std::endl;
	const char* c_str = ("clos " +std::to_string(getpid()) + " "  + std::to_string(fd)).c_str();
    sem_wait(sem_requests);
	memcpy(buf_requests.buf + *buf_requests.i, c_str, strlen(c_str) + 1);
	char tmp_str[1024];
	printf("c_str %s, len c_str %i\n", c_str, strlen(c_str));
	sprintf(tmp_str, "%s", (char*) buf_requests.buf + *buf_requests.i);
	printf("add read msg sent: %s\n", tmp_str);
	*buf_requests.i = *buf_requests.i + strlen(c_str) + 1;
	std::cout << "*buf_requests.i == " << *buf_requests.i << std::endl;
	sem_post(sem_requests);
	sem_post(sem_new_msgs);
	return 0;
}

int add_read_request(int fd, size_t count) {
	std::cout << "read request" << std::endl;
	std::string str = "read " + std::to_string(getpid()) + " " + std::to_string(fd) + " " + std::to_string(count);
	const char* c_str = str.c_str();
    sem_wait(sem_requests);
	memcpy(buf_requests.buf + *buf_requests.i, c_str, strlen(c_str) + 1);
	char tmp_str[1024];
	printf("c_str %s, len c_str %i\n", c_str, strlen(c_str));
	sprintf(tmp_str, "%s", (char*) buf_requests.buf + *buf_requests.i);
	printf("add read msg sent: %s\n", tmp_str);
	*buf_requests.i = *buf_requests.i + strlen(c_str) + 1;
	std::cout << "*buf_requests.i == " << *buf_requests.i << std::endl;
	sem_post(sem_requests);
	sem_post(sem_new_msgs);
	//read response (offest)
	std::cout << "read before wait sem response" << std::endl;
	sem_wait(sem_response);
	std::cout << "read after response" << std::endl;
	int offset = buf_response[i_resp];
	++i_resp;
	return offset;
}

void add_write_request(int fd, size_t count) {
	std::cout << "write request" << std::endl;
	std::string str = "writ " + std::to_string(getpid()) +  " " + std::to_string(fd) + " " + std::to_string(count);
	const char* c_str = str.c_str();    
	sem_wait(sem_requests);
	memcpy(buf_requests.buf + *buf_requests.i, c_str, strlen(c_str) + 1);
	std::cout << "write before response" << std::endl;
	std::cout << "i: " << *buf_requests.i << std::endl;
	char tmp_str[1024];
	printf("c_str %s, len c_str %i\n", c_str, strlen(c_str));
	sprintf(tmp_str, "%s", (char*) buf_requests.buf + *buf_requests.i);
	printf("add write msg sent: %s\n", tmp_str);
	*buf_requests.i = *buf_requests.i + strlen(c_str) + 1;
	std::cout << "*buf_requests.i == " << *buf_requests.i << std::endl;
	sem_post(sem_requests);
	sem_post(sem_new_msgs);
	
	//read response (offest)
	return;
}

void read_shm(void* shm, int offset, void* buffer, size_t count) {
	memcpy(buffer, shm + offset, count); 
}

void write_shm(void* shm, size_t offset, const void* buffer, size_t count) {	
	std::cout << "before wrote offset" << offset << " count " << count <<std::endl;
	memcpy(shm + offset, buffer, count); 
	std::cout << "after wrote " << count << " bytes" << std::endl;
}
		

extern "C" {

int open(const char *pathname, int flags, ...) {
	printf("Opening of the file %s captured\n", pathname);
	if (real_open == NULL)
		mtrace_init();
	const char* prefix = "file_";
	const char* prefix_2 = "output_file_";
	if (strncmp("file_", pathname, strlen(prefix)) == 0 || strncmp("output_file_", pathname, strlen(prefix_2)) == 0) {
		printf("calling my open...\n");
		//create shm
		int fd = add_open_request(pathname);
		files[fd] = std::pair<void*, int>(get_shm(pathname), 0);
		std::cout << "result of add_open_request, fd: " << fd << std::endl;
		capio_files_descriptors.insert(fd);
		capio_files_paths.insert(pathname);
		return fd;
	}
	else {
		printf("calling real open...\n");
		mode_t mode = 0;
		if ((flags & O_CREAT) || (flags & O_TMPFILE) == O_TMPFILE) {
			va_list ap;
			va_start(ap, flags);
			mode = va_arg(ap, mode_t);
			va_end(ap);
		}
		int fd = real_open(pathname, flags, mode);
		printf("fd %i \n", fd);
		return fd;
	}
}



int close(int fd) {
	if (real_close == NULL)
		mtrace_init();
	printf("Closing of the file %i captured\n", fd);
	if (capio_files_descriptors.find(fd) != capio_files_descriptors.end()) {
		printf("calling my close...\n");
		//TODO: only the CAPIO deamon will free the shared memory
		int res = add_close_request(fd);
		capio_files_descriptors.erase(fd);
		return res;
	}
	else {
		printf("calling real close...\n");
		int res = real_close(fd);
		printf("result of real close: %i\n", res);
		return res;
	}
}

ssize_t read(int fd, void *buffer, size_t count) {
	printf("reading of the file %i captured\n", fd);
	if (capio_files_descriptors.find(fd) != capio_files_descriptors.end()) {
		printf("calling my read...\n");
		int offset = add_read_request(fd, count);
		read_shm(files[fd].first, offset, buffer, count);
		files[fd].second = offset;
		return count;
	}
	else { 
		printf("calling real read...\n");
		return real_read(fd, buffer, count);
	}
}

ssize_t write(int fd, const  void *buffer, size_t count) {
	printf("writing of the file %i captured\n", fd);
	if (capio_files_descriptors.find(fd) != capio_files_descriptors.end()) {
		add_write_request(fd, count);
		if (files.find(fd) == files.end()) { //only for debug
			std::cout << "error write to invalid adress" << std::endl;
			exit(1);
		}
		write_shm(files[fd].first, files[fd].second, buffer, count);
		files[fd].second += count;
		return count;
	}
	else {
		printf("calling real write\n");
		return real_write(fd, buffer, count);
	}
}

int access(const char *pathname, int mode) {
	if (real_access == NULL)
		mtrace_init();
	printf("access of the file %s captured\n", pathname);
	if (capio_files_paths.find(pathname) != capio_files_paths.end()) {
		return 0;
	}
	else {
		printf("calling real access\n");
		return real_access(pathname, mode);
	}
}

int stat(const char *__restrict pathname, struct stat *__restrict statbuf) {
	if (real_stat == NULL)
		mtrace_init();
	printf("stat of the file %s captured\n", pathname);
	if (capio_files_paths.find(pathname) != capio_files_paths.end()) {
		return 0;	
	}
	else {
		printf("calling real stat\n");
		return real_stat(pathname, statbuf);
	}
}

int fstat(int fd, struct stat *statbuf) { 
	if (real_fstat == NULL)
		mtrace_init();
	printf("fstat of the file %i captured\n", fd);
	if (capio_files_descriptors.find(fd) != capio_files_descriptors.end()) {
		std::cout << "calling my fstat" << std::endl;
		return 0;
	}
	else {
		printf("calling real fstat\n");
		return real_fstat(fd, statbuf);
	}
}


}
