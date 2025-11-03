#include "inc/def.h"
#include <arpa/inet.h>
#include <assert.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netpacket/packet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <threads.h>
#include <unistd.h>

static volatile bool quit;
static void sigh(int) { quit = 1; }

static int socketh;
static const u64 local = 0x222222222222;

static Smem *shm;
static int sendh(void *) {
  struct sockaddr_ll addr = {AF_PACKET, 0, 1, 0, 0, 6, "\6"};
  void *buf = &sbuf;
  while (!quit) {
    auto l = sendto(socketh, buf, sizeof(sbuf), 0, (struct sockaddr *)&addr,
                    sizeof(addr));
    if (l == -1) {
      perror("sendto");
      break;
    }
  }
  quit = 1;
  puts("send exit");
  return 0;
}
static int recvh(void *) {
  u16 seq = 0;
  u64 loss = 0;
  while (!quit) {
    auto l = recvfrom(socketh, (void *)&rbuf, sizeof(rbuf), 0, 0, 0);
    if (l == -1) {
      perror("recvfrom");
      break;
    }
    if (rbuf.p != 0x1919 || rbuf.src == local)
      continue;
    u16 s = rbuf.seq;
    u64 lo = s - seq - 1;
    seq = s;
    loss += lo;
    if (lo)
      printf("loss:%d\n", loss);
  }
  quit = 1;
  puts("recv exit");
  return 0;
}
int main() {
  socketh = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  assert(socketh != -1);
  puts("eth");

  signal(SIGINT, sigh);
  signal(SIGTERM, sigh);
  thrd_t sendt, recvt;
#define crthrd(fn) thrd_create(&fn##t, fn##h, 0)
  crthrd(send);
  crthrd(recv);

  thrd_join(sendt, 0);
  thrd_join(recvt, 0);
  close(socketh);
}
