/*
 * Copyright (c) 1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997
 *     The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from the Stanford/CMU enet packet filter,
 * (net/enet.c) distributed as part of 4.3BSD, and code contributed
 * to Berkeley by Steven McCanne and Van Jacobson both of Lawrence
 * Berkeley Laboratory.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *     This product includes software developed by the University of
 *     California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *     @(#)bpf.c       7.5 (Berkeley) 7/15/91
 */

#if !(defined(lint) || defined(KERNEL) || defined(_KERNEL))
static const char rcsid[] _U_ =
    "@(#) $Header: /tcpdump/master/libpcap/bpf_filter_linux.c,v 1.46 2008-01-02 04:16:46 guy Exp $ (LBL)";
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_INTTYPES_H
#include <inttypes.h>
#elif HAVE_STDINT_H
#include <stdint.h>
#endif
#ifdef HAVE_SYS_BITYPES_H
#include <sys/bitypes.h>
#endif

#include <sys/param.h>
#include <sys/types.h>
#include <sys/time.h>

#define        SOLARIS (defined(sun) && (defined(__SVR4) || defined(__svr4__)))
#if defined(__hpux) || SOLARIS
# include <sys/sysmacros.h>
# include <sys/stream.h>
# define       mbuf    msgb
# define       m_next  b_cont
# define       MLEN(m) ((m)->b_wptr - (m)->b_rptr)
# define       mtod(m,t)       ((t)(m)->b_rptr)
#else /* defined(__hpux) || SOLARIS */
# define       MLEN(m) ((m)->m_len)
#endif /* defined(__hpux) || SOLARIS */

#include <pcap/bpf.h>
#include <linux/filter.h>
#include <linux/if_packet.h>

#if !defined(KERNEL) && !defined(_KERNEL)
#include <stdlib.h>
#endif

#define int32 bpf_int32
#define u_int32 bpf_u_int32

#ifndef LBL_ALIGN
/*
 * XXX - IA-64?  If not, this probably won't work on Win64 IA-64
 * systems, unless LBL_ALIGN is defined elsewhere for them.
 * XXX - SuperH?  If not, this probably won't work on WinCE SuperH
 * systems, unless LBL_ALIGN is defined elsewhere for them.
 */
#if defined(sparc) || defined(__sparc__) || defined(mips) || \
    defined(ibm032) || defined(__alpha) || defined(__hpux) || \
    defined(__arm__)
#define LBL_ALIGN
#endif
#endif

#ifndef LBL_ALIGN
#include <netinet/in.h>

#define EXTRACT_SHORT(p)       ((u_short)ntohs(*(u_short *)p))
#define EXTRACT_LONG(p)                (ntohl(*(u_int32 *)p))
#else
#define EXTRACT_SHORT(p)\
       ((u_short)\
               ((u_short)*((u_char *)p+0)<<8|\
                (u_short)*((u_char *)p+1)<<0))
#define EXTRACT_LONG(p)\
               ((u_int32)*((u_char *)p+0)<<24|\
                (u_int32)*((u_char *)p+1)<<16|\
                (u_int32)*((u_char *)p+2)<<8|\
                (u_int32)*((u_char *)p+3)<<0)
#endif

#if defined(KERNEL) || defined(_KERNEL)
# if !defined(__hpux) && !SOLARIS
#include <sys/mbuf.h>
# endif
#define MINDEX(len, _m, _k) \
{ \
       len = MLEN(m); \
       while ((_k) >= len) { \
               (_k) -= len; \
               (_m) = (_m)->m_next; \
               if ((_m) == 0) \
                       return 0; \
               len = MLEN(m); \
       } \
}

static int
m_xword(m, k, err)
       register struct mbuf *m;
       register int k, *err;
{
       register int len;
       register u_char *cp, *np;
       register struct mbuf *m0;

       MINDEX(len, m, k);
       cp = mtod(m, u_char *) + k;
       if (len - k >= 4) {
               *err = 0;
               return EXTRACT_LONG(cp);
       }
       m0 = m->m_next;
       if (m0 == 0 || MLEN(m0) + len - k < 4)
               goto bad;
       *err = 0;
       np = mtod(m0, u_char *);
       switch (len - k) {

       case 1:
               return (cp[0] << 24) | (np[0] << 16) | (np[1] << 8) | np[2];

       case 2:
               return (cp[0] << 24) | (cp[1] << 16) | (np[0] << 8) | np[1];

       default:
               return (cp[0] << 24) | (cp[1] << 16) | (cp[2] << 8) | np[0];
       }
    bad:
       *err = 1;
       return 0;
}

static int
m_xhalf(m, k, err)
       register struct mbuf *m;
       register int k, *err;
{
       register int len;
       register u_char *cp;
       register struct mbuf *m0;

       MINDEX(len, m, k);
       cp = mtod(m, u_char *) + k;
       if (len - k >= 2) {
               *err = 0;
               return EXTRACT_SHORT(cp);
       }
       m0 = m->m_next;
       if (m0 == 0)
               goto bad;
       *err = 0;
       return (cp[0] << 8) | mtod(m0, u_char *)[0];
 bad:
       *err = 1;
       return 0;
}
#endif /* KERNEL or _KERNEL */

enum {
       /* Ancillary data */
       BPF_S_ANC_NONE,
       BPF_S_ANC_PROTOCOL,
       BPF_S_ANC_PKTTYPE,
       BPF_S_ANC_IFINDEX,
       BPF_S_ANC_NLATTR,
       BPF_S_ANC_NLATTR_NEST,
       BPF_S_ANC_MARK,
       BPF_S_ANC_QUEUE,
       BPF_S_ANC_HATYPE,
       BPF_S_ANC_RXHASH,
       BPF_S_ANC_CPU,
       BPF_S_ANC_ALU_XOR_X,
       BPF_S_ANC_SECCOMP_LD_W,
       BPF_S_ANC_VLAN_TAG,
       BPF_S_ANC_VLAN_TAG_PRESENT,
};

/* ntop */
#ifndef TP_STATUS_VLAN_VALID  
#define TP_STATUS_VLAN_VALID            (1 << 4) /* auxdata has valid tp_vlan_tci */
#endif

/*
 * Execute the filter program starting at pc on the packet p
 * wirelen is the length of the original packet
 * buflen is the amount of data present
 * For the kernel, p is assumed to be a pointer to an mbuf if buflen is 0,
 * in all other cases, p is a pointer to a buffer and buflen is its size.
 */
u_int
bpf_filter_linux(pc, p, tp_vlan_tci, wirelen, buflen)
       register const struct bpf_insn *pc;
       register const u_char *p;
        u_int16_t tp_vlan_tci;
       u_int wirelen;
       register u_int buflen;
{
       register u_int32 A, X;
       register int k;
       int32 mem[BPF_MEMWORDS];
#if defined(KERNEL) || defined(_KERNEL)
       struct mbuf *m, *n;
       int merr, len;

       if (buflen == 0) {
               m = (struct mbuf *)p;
               p = mtod(m, u_char *);
               buflen = MLEN(m);
       } else
               m = NULL;
#endif

       if (pc == 0)
               /*
                * No filter means accept all.
                */
               return (u_int)-1;
       A = 0;
       X = 0;
       --pc;
       while (1) {
               ++pc;
               switch (pc->code) {

               default:
#if defined(KERNEL) || defined(_KERNEL)
                       return 0;
#else
                       abort();
#endif
               case BPF_RET|BPF_K:
                       return (u_int)pc->k;

               case BPF_RET|BPF_A:
                       return (u_int)A;

               case BPF_LD|BPF_W|BPF_ABS:
                       k = pc->k;
                       if (k + sizeof(int32) > buflen) {
#if defined(KERNEL) || defined(_KERNEL)
                               if (m == NULL)
                                       return 0;
                               A = m_xword(m, k, &merr);
                               if (merr != 0)
                                       return 0;
                               continue;
#else
                               return 0;
#endif
                       }
                       A = EXTRACT_LONG(&p[k]);
                       continue;

               case BPF_LD|BPF_H|BPF_ABS:
                       k = pc->k;
                       if (k + sizeof(short) > buflen) {
#if defined(KERNEL) || defined(_KERNEL)
                               if (m == NULL)
                                       return 0;
                               A = m_xhalf(m, k, &merr);
                               if (merr != 0)
                                       return 0;
                               continue;
#else
                               return 0;
#endif
                       }
                       A = EXTRACT_SHORT(&p[k]);
                       continue;

               case BPF_LD|BPF_B|BPF_ABS:
               {
#if defined(SKF_AD_VLAN_TAG) && defined(SKF_AD_VLAN_TAG_PRESENT)
                       int code = BPF_S_ANC_NONE;
#define ANCILLARY(CODE) case SKF_AD_OFF + SKF_AD_##CODE:       \
                               code = BPF_S_ANC_##CODE;        \
                               break
                       switch (pc->k) {
/*                     ANCILLARY(PROTOCOL);
                       ANCILLARY(PKTTYPE);
                       ANCILLARY(IFINDEX);
                       ANCILLARY(NLATTR);
                       ANCILLARY(NLATTR_NEST);
                       ANCILLARY(MARK);
                       ANCILLARY(QUEUE);
                       ANCILLARY(HATYPE);
                       ANCILLARY(RXHASH);
                       ANCILLARY(CPU);
                       ANCILLARY(ALU_XOR_X); */
                       ANCILLARY(VLAN_TAG);
                       ANCILLARY(VLAN_TAG_PRESENT);
                        default :
#endif
                          k = pc->k;
                          if (k >= buflen) {
#if defined(KERNEL) || defined(_KERNEL)
                             if (m == NULL)
                                return 0;
                             n = m;
                             MINDEX(len, n, k);
                             A = mtod(n, u_char *)[k];
                             continue;
#else
                             return 0;
#endif
                          }
                          A = p[k];
#if defined(SKF_AD_VLAN_TAG) && defined(SKF_AD_VLAN_TAG_PRESENT)
                       }
                       switch (code) {
                       case BPF_S_ANC_VLAN_TAG:
                               A = tp_vlan_tci & ~TP_STATUS_VLAN_VALID;
                               break;

                       case BPF_S_ANC_VLAN_TAG_PRESENT:
                               A = !!(tp_vlan_tci & TP_STATUS_VLAN_VALID);
                               break;
                       }
#endif
                       continue;
               }
               case BPF_LD|BPF_W|BPF_LEN:
                       A = wirelen;
                       continue;

               case BPF_LDX|BPF_W|BPF_LEN:
                       X = wirelen;
                       continue;

               case BPF_LD|BPF_W|BPF_IND:
                       k = X + pc->k;
                       if (k + sizeof(int32) > buflen) {
#if defined(KERNEL) || defined(_KERNEL)
                               if (m == NULL)
                                       return 0;
                               A = m_xword(m, k, &merr);
                               if (merr != 0)
                                       return 0;
                               continue;
#else
                               return 0;
#endif
                       }
                       A = EXTRACT_LONG(&p[k]);
                       continue;

               case BPF_LD|BPF_H|BPF_IND:
                       k = X + pc->k;
                       if (k + sizeof(short) > buflen) {
#if defined(KERNEL) || defined(_KERNEL)
                               if (m == NULL)
                                       return 0;
                               A = m_xhalf(m, k, &merr);
                               if (merr != 0)
                                       return 0;
                               continue;
#else
                               return 0;
#endif
                       }
                       A = EXTRACT_SHORT(&p[k]);
                       continue;

               case BPF_LD|BPF_B|BPF_IND:
                       k = X + pc->k;
                       if (k >= buflen) {
#if defined(KERNEL) || defined(_KERNEL)
                               if (m == NULL)
                                       return 0;
                               n = m;
                               MINDEX(len, n, k);
                               A = mtod(n, u_char *)[k];
                               continue;
#else
                               return 0;
#endif
                       }
                       A = p[k];
                       continue;

               case BPF_LDX|BPF_MSH|BPF_B:
                       k = pc->k;
                       if (k >= buflen) {
#if defined(KERNEL) || defined(_KERNEL)
                               if (m == NULL)
                                       return 0;
                               n = m;
                               MINDEX(len, n, k);
                               X = (mtod(n, char *)[k] & 0xf) << 2;
                               continue;
#else
                               return 0;
#endif
                       }
                       X = (p[pc->k] & 0xf) << 2;
                       continue;

               case BPF_LD|BPF_IMM:
                       A = pc->k;
                       continue;

               case BPF_LDX|BPF_IMM:
                       X = pc->k;
                       continue;

               case BPF_LD|BPF_MEM:
                       A = mem[pc->k];
                       continue;

               case BPF_LDX|BPF_MEM:
                       X = mem[pc->k];
                       continue;

               case BPF_ST:
                       mem[pc->k] = A;
                       continue;

               case BPF_STX:
                       mem[pc->k] = X;
                       continue;

               case BPF_JMP|BPF_JA:
                       pc += pc->k;
                       continue;

               case BPF_JMP|BPF_JGT|BPF_K:
                       pc += (A > pc->k) ? pc->jt : pc->jf;
                       continue;

               case BPF_JMP|BPF_JGE|BPF_K:
                       pc += (A >= pc->k) ? pc->jt : pc->jf;
                       continue;

               case BPF_JMP|BPF_JEQ|BPF_K:
                       pc += (A == pc->k) ? pc->jt : pc->jf;
                       continue;

               case BPF_JMP|BPF_JSET|BPF_K:
                       pc += (A & pc->k) ? pc->jt : pc->jf;
                       continue;

               case BPF_JMP|BPF_JGT|BPF_X:
                       pc += (A > X) ? pc->jt : pc->jf;
                       continue;

               case BPF_JMP|BPF_JGE|BPF_X:
                       pc += (A >= X) ? pc->jt : pc->jf;
                       continue;

               case BPF_JMP|BPF_JEQ|BPF_X:
                       pc += (A == X) ? pc->jt : pc->jf;
                       continue;

               case BPF_JMP|BPF_JSET|BPF_X:
                       pc += (A & X) ? pc->jt : pc->jf;
                       continue;

               case BPF_ALU|BPF_ADD|BPF_X:
                       A += X;
                       continue;

               case BPF_ALU|BPF_SUB|BPF_X:
                       A -= X;
                       continue;

               case BPF_ALU|BPF_MUL|BPF_X:
                       A *= X;
                       continue;

               case BPF_ALU|BPF_DIV|BPF_X:
                       if (X == 0)
                               return 0;
                       A /= X;
                       continue;

               case BPF_ALU|BPF_AND|BPF_X:
                       A &= X;
                       continue;

               case BPF_ALU|BPF_OR|BPF_X:
                       A |= X;
                       continue;

               case BPF_ALU|BPF_LSH|BPF_X:
                       A <<= X;
                       continue;

               case BPF_ALU|BPF_RSH|BPF_X:
                       A >>= X;
                       continue;

               case BPF_ALU|BPF_ADD|BPF_K:
                       A += pc->k;
                       continue;

               case BPF_ALU|BPF_SUB|BPF_K:
                       A -= pc->k;
                       continue;

               case BPF_ALU|BPF_MUL|BPF_K:
                       A *= pc->k;
                       continue;

               case BPF_ALU|BPF_DIV|BPF_K:
                       A /= pc->k;
                       continue;

               case BPF_ALU|BPF_AND|BPF_K:
                       A &= pc->k;
                       continue;

               case BPF_ALU|BPF_OR|BPF_K:
                       A |= pc->k;
                       continue;

               case BPF_ALU|BPF_LSH|BPF_K:
                       A <<= pc->k;
                       continue;

               case BPF_ALU|BPF_RSH|BPF_K:
                       A >>= pc->k;
                       continue;

               case BPF_ALU|BPF_NEG:
                       A = -A;
                       continue;

               case BPF_MISC|BPF_TAX:
                       X = A;
                       continue;

               case BPF_MISC|BPF_TXA:
                       A = X;
                       continue;

               }
       }
}

