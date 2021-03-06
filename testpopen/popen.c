#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../gtest/gtest.h"


extern int timed_read(const char* command, char* result, int size);
TEST(TIMEDREAD, CASE1)
{
	char result[4096];
	ASSERT_EQ(1, timed_read("aaa", result, 4096));
	EXPECT_EQ(-1, timed_read("aaabbbb", result, 4096));
	EXPECT_EQ(-1, timed_read("ls", result, 4096));
	EXPECT_STREQ("a",result);
	
}


int timed_read(const char* command, char* result, int size) 
{
        int pipe_fd[2];
        pid_t pid;
        int ret = pipe(pipe_fd);
        if (ret < 0)  return -1;
        pid = fork();        
        if (pid > 0)
        {
                close(pipe_fd[1]);
                struct timeval tv;
                int nready = 0;
                tv.tv_sec = 5;
                tv.tv_usec = 0;
                fd_set rset;
                FD_ZERO(&rset);
                FD_SET(pipe_fd[0], &rset);
                nready = select(pipe_fd[0] + 1, &rset, NULL, NULL, &tv);
                if (nready > 0 && FD_ISSET(pipe_fd[0], &rset)) {
                        //printf("ready\n");
                        int count = read(pipe_fd[0], result, size - 1);
                        if (count < 0) 
                                count = 0;
                        result[count] = '\0';
                        char *loc = index(result, '\n');
                        if (loc) 
                                result[loc - result] = '\0';
                } else if (nready <= 0) {
			return -1;
		}
                //wait 5 seconds for data ready.
                kill(pid, 9);
                wait(NULL);
                return 0;
        } else if (pid == 0){
                close(pipe_fd[0]);
                dup2(pipe_fd[1], 1);
                dup2(pipe_fd[1], 2);
                close(pipe_fd[1]);
                execlp("sh", "sh", "-c", command);
                _exit(-1);
        } else {
                return -1;
        }
}

int main(int argc, char **argv) {
        char result[4096];
	::testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();
	printf("*****\n");
        timed_read("/usr/bin/ls", result, 4096);
	printf("%s\n", result);
}



