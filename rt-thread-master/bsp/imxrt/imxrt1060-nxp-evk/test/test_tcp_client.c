/**
  *****************************************************************************
  * @file    test_tcp_client.c
  * @author  zwx
  * @version V1.0.0
  * @date    2019-07-25
  * @brief   
  ******************************************************************************
  */  
#include "rtthread.h"
#include "lwip/sockets.h"

#define DRV_DEBUG
#define LOG_TAG             "drv.tcp_client"
#include <drv_log.h>

#define TEST_BUF_SIZE  128
uint8_t *pSendBuf, *pRecvBuf;

int test_tcp_client(void)
{
	int i = 0;
	int sockfd=0;
	struct sockaddr_in servaddr;
	int recvn = 0;
	int status = 0;
	int sendn = 0;
	int flags = 0;
	
	pSendBuf = rt_malloc(TEST_BUF_SIZE);
	if(pSendBuf == NULL)
	{
		LOG_E("[tcp_client] pSendBuf alloc failed\n");
		return -1;
	}
	pRecvBuf = rt_malloc(TEST_BUF_SIZE);
	if(pRecvBuf == NULL)
	{
		LOG_E("[tcp_client] pRecvBuf alloc failed\n");
		return -1;
	}	
	
	rt_strncpy((char *)pSendBuf, "tcp client data\n", 16);
	
	while(1)
	{
		/* 创建socket */
		sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
		if(sockfd < 0)
		{
			LOG_E("[tcp_client] socket fail\r\n");
			continue;
		}	
		else
		{
			LOG_D("[tcp_client] socket ok\r\n");
		}
		
		/* 设置要连接的服务器端的ip和port信息 */
		servaddr.sin_addr.s_addr = inet_addr("192.168.1.133");
		servaddr.sin_family = AF_INET;
		servaddr.sin_len = sizeof(servaddr);
		servaddr.sin_port = htons(8200);	
			
		/* 连接到服务器 */
		status = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr));  /* 阻塞情况 超时时间为18s */
		if (status != 0)
		{
			LOG_E("[tcp_client] connetc failed %d\r\n", status);
			close(sockfd);
			continue;
		}
		else
		{
			LOG_D("[tcp_client] connetc ok\r\n");
		}		
		
		while(1)
		{
			recvn = recv(sockfd, pRecvBuf, TEST_BUF_SIZE, 0);
			if(recvn == -1)
			{
				LOG_E("[tcp_client] recv Error State = %d\r\n", recvn);
				close(sockfd);
				break;					
			}
			else if(recvn == 0)
			{
				LOG_E("[tcp_client] recv remote close Error\r\n");
				close(sockfd);
				break;
			}
			else
			{
				rt_kprintf("[tcp_client] Recv : ");
				for(i=0; i<recvn; i++)
				{
					rt_kprintf("%c", pRecvBuf[i]);
				}
				rt_kprintf("\n");
				
				send(sockfd, pSendBuf, 16, 0);
			}		
		}		
	}			
}

MSH_CMD_EXPORT(test_tcp_client, test_tcp_client);

