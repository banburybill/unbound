/*
 * testcode/unitmain.c - unit test main program for unbound.
 *
 * Copyright (c) 2007, NLnet Labs. All rights reserved.
 *
 * This software is open source.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of the NLNET LABS nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/**
 * \file
 * Unit test main program. Calls all the other unit tests.
 * Exits with code 1 on a failure. 0 if all unit tests are successful.
 */

#include "config.h"
#ifdef HAVE_OPENSSL_ERR_H
#include <openssl/err.h>
#endif

#ifdef HAVE_OPENSSL_RAND_H
#include <openssl/rand.h>
#endif

#ifdef HAVE_OPENSSL_CONF_H
#include <openssl/conf.h>
#endif

#ifdef HAVE_OPENSSL_ENGINE_H
#include <openssl/engine.h>
#endif

#ifdef HAVE_NSS
/* nss3 */
#include "nss.h"
#endif

#include "sldns/rrdef.h"
#include "sldns/keyraw.h"
#include "util/log.h"
#include "testcode/unitmain.h"

/** number of tests done */
int testcount = 0;

#include "util/alloc.h"
/** test alloc code */
static void
alloc_test(void) {
	alloc_special_type *t1, *t2;
	struct alloc_cache major, minor1, minor2;
	int i;

	unit_show_feature("alloc_special_obtain");
	alloc_init(&major, NULL, 0);
	alloc_init(&minor1, &major, 0);
	alloc_init(&minor2, &major, 1);

	t1 = alloc_special_obtain(&minor1);
	alloc_clear(&minor1);

	alloc_special_release(&minor2, t1);
	t2 = alloc_special_obtain(&minor2);
	unit_assert( t1 == t2 ); /* reused */
	alloc_special_release(&minor2, t1);

	for(i=0; i<100; i++) {
		t1 = alloc_special_obtain(&minor1);
		alloc_special_release(&minor2, t1);
	}
	if(0) {
		alloc_stats(&minor1);
		alloc_stats(&minor2);
		alloc_stats(&major);
	}
	/* reuse happened */
	unit_assert(minor1.num_quar + minor2.num_quar + major.num_quar == 11);

	alloc_clear(&minor1);
	alloc_clear(&minor2);
	unit_assert(major.num_quar == 11);
	alloc_clear(&major);
}

#include "util/net_help.h"
/** test net code */
static void 
net_test(void)
{
	const char* t4[] = {"\000\000\000\000",
		"\200\000\000\000",
		"\300\000\000\000",
		"\340\000\000\000",
		"\360\000\000\000",
		"\370\000\000\000",
		"\374\000\000\000",
		"\376\000\000\000",
		"\377\000\000\000",
		"\377\200\000\000",
		"\377\300\000\000",
		"\377\340\000\000",
		"\377\360\000\000",
		"\377\370\000\000",
		"\377\374\000\000",
		"\377\376\000\000",
		"\377\377\000\000",
		"\377\377\200\000",
		"\377\377\300\000",
		"\377\377\340\000",
		"\377\377\360\000",
		"\377\377\370\000",
		"\377\377\374\000",
		"\377\377\376\000",
		"\377\377\377\000",
		"\377\377\377\200",
		"\377\377\377\300",
		"\377\377\377\340",
		"\377\377\377\360",
		"\377\377\377\370",
		"\377\377\377\374",
		"\377\377\377\376",
		"\377\377\377\377",
		"\377\377\377\377",
		"\377\377\377\377",
	};
	unit_show_func("util/net_help.c", "str_is_ip6");
	unit_assert( str_is_ip6("::") );
	unit_assert( str_is_ip6("::1") );
	unit_assert( str_is_ip6("2001:7b8:206:1:240:f4ff:fe37:8810") );
	unit_assert( str_is_ip6("fe80::240:f4ff:fe37:8810") );
	unit_assert( !str_is_ip6("0.0.0.0") );
	unit_assert( !str_is_ip6("213.154.224.12") );
	unit_assert( !str_is_ip6("213.154.224.255") );
	unit_assert( !str_is_ip6("255.255.255.0") );
	unit_show_func("util/net_help.c", "is_pow2");
	unit_assert( is_pow2(0) );
	unit_assert( is_pow2(1) );
	unit_assert( is_pow2(2) );
	unit_assert( is_pow2(4) );
	unit_assert( is_pow2(8) );
	unit_assert( is_pow2(16) );
	unit_assert( is_pow2(1024) );
	unit_assert( is_pow2(1024*1024) );
	unit_assert( is_pow2(1024*1024*1024) );
	unit_assert( !is_pow2(3) );
	unit_assert( !is_pow2(5) );
	unit_assert( !is_pow2(6) );
	unit_assert( !is_pow2(7) );
	unit_assert( !is_pow2(9) );
	unit_assert( !is_pow2(10) );
	unit_assert( !is_pow2(11) );
	unit_assert( !is_pow2(17) );
	unit_assert( !is_pow2(23) );
	unit_assert( !is_pow2(257) );
	unit_assert( !is_pow2(259) );

	/* test addr_mask */
	unit_show_func("util/net_help.c", "addr_mask");
	if(1) {
		struct sockaddr_in a4;
		struct sockaddr_in6 a6;
		socklen_t l4 = (socklen_t)sizeof(a4);
		socklen_t l6 = (socklen_t)sizeof(a6);
		int i;
		a4.sin_family = AF_INET;
		a6.sin6_family = AF_INET6;
		for(i=0; i<35; i++) {
			/* address 255.255.255.255 */
			memcpy(&a4.sin_addr, "\377\377\377\377", 4);
			addr_mask((struct sockaddr_storage*)&a4, l4, i);
			unit_assert(memcmp(&a4.sin_addr, t4[i], 4) == 0);
		}
		memcpy(&a6.sin6_addr, "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377", 16);
		addr_mask((struct sockaddr_storage*)&a6, l6, 128);
		unit_assert(memcmp(&a6.sin6_addr, "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377", 16) == 0);
		addr_mask((struct sockaddr_storage*)&a6, l6, 122);
		unit_assert(memcmp(&a6.sin6_addr, "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\300", 16) == 0);
		addr_mask((struct sockaddr_storage*)&a6, l6, 120);
		unit_assert(memcmp(&a6.sin6_addr, "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\000", 16) == 0);
		addr_mask((struct sockaddr_storage*)&a6, l6, 64);
		unit_assert(memcmp(&a6.sin6_addr, "\377\377\377\377\377\377\377\377\000\000\000\000\000\000\000\000", 16) == 0);
		addr_mask((struct sockaddr_storage*)&a6, l6, 0);
		unit_assert(memcmp(&a6.sin6_addr, "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000", 16) == 0);
	}

	/* test addr_in_common */
	unit_show_func("util/net_help.c", "addr_in_common");
	if(1) {
		struct sockaddr_in a4, b4;
		struct sockaddr_in6 a6, b6;
		socklen_t l4 = (socklen_t)sizeof(a4);
		socklen_t l6 = (socklen_t)sizeof(a6);
		int i;
		a4.sin_family = AF_INET;
		b4.sin_family = AF_INET;
		a6.sin6_family = AF_INET6;
		b6.sin6_family = AF_INET6;
		memcpy(&a4.sin_addr, "abcd", 4);
		memcpy(&b4.sin_addr, "abcd", 4);
		unit_assert(addr_in_common((struct sockaddr_storage*)&a4, 32,
			(struct sockaddr_storage*)&b4, 32, l4) == 32);
		unit_assert(addr_in_common((struct sockaddr_storage*)&a4, 34,
			(struct sockaddr_storage*)&b4, 32, l4) == 32);
		for(i=0; i<=32; i++) {
			unit_assert(addr_in_common(
				(struct sockaddr_storage*)&a4, 32,
				(struct sockaddr_storage*)&b4, i, l4) == i);
			unit_assert(addr_in_common(
				(struct sockaddr_storage*)&a4, i,
				(struct sockaddr_storage*)&b4, 32, l4) == i);
			unit_assert(addr_in_common(
				(struct sockaddr_storage*)&a4, i,
				(struct sockaddr_storage*)&b4, i, l4) == i);
		}
		for(i=0; i<=32; i++) {
			memcpy(&a4.sin_addr, "\377\377\377\377", 4);
			memcpy(&b4.sin_addr, t4[i], 4);
			unit_assert(addr_in_common(
				(struct sockaddr_storage*)&a4, 32,
				(struct sockaddr_storage*)&b4, 32, l4) == i);
			unit_assert(addr_in_common(
				(struct sockaddr_storage*)&b4, 32,
				(struct sockaddr_storage*)&a4, 32, l4) == i);
		}
		memcpy(&a6.sin6_addr, "abcdefghabcdefgh", 16);
		memcpy(&b6.sin6_addr, "abcdefghabcdefgh", 16);
		unit_assert(addr_in_common((struct sockaddr_storage*)&a6, 128,
			(struct sockaddr_storage*)&b6, 128, l6) == 128);
		unit_assert(addr_in_common((struct sockaddr_storage*)&a6, 129,
			(struct sockaddr_storage*)&b6, 128, l6) == 128);
		for(i=0; i<=128; i++) {
			unit_assert(addr_in_common(
				(struct sockaddr_storage*)&a6, 128,
				(struct sockaddr_storage*)&b6, i, l6) == i);
			unit_assert(addr_in_common(
				(struct sockaddr_storage*)&a6, i,
				(struct sockaddr_storage*)&b6, 128, l6) == i);
			unit_assert(addr_in_common(
				(struct sockaddr_storage*)&a6, i,
				(struct sockaddr_storage*)&b6, i, l6) == i);
		}
	}
	/* test sockaddr_cmp_addr */
	unit_show_func("util/net_help.c", "sockaddr_cmp_addr");
	if(1) {
		struct sockaddr_storage a, b;
		socklen_t alen = (socklen_t)sizeof(a);
		socklen_t blen = (socklen_t)sizeof(b);
		unit_assert(ipstrtoaddr("127.0.0.0", 53, &a, &alen));
		unit_assert(ipstrtoaddr("127.255.255.255", 53, &b, &blen));
		unit_assert(sockaddr_cmp_addr(&a, alen, &b, blen) < 0);
		unit_assert(sockaddr_cmp_addr(&b, blen, &a, alen) > 0);
		unit_assert(sockaddr_cmp_addr(&a, alen, &a, alen) == 0);
		unit_assert(sockaddr_cmp_addr(&b, blen, &b, blen) == 0);
		unit_assert(ipstrtoaddr("192.168.121.5", 53, &a, &alen));
		unit_assert(sockaddr_cmp_addr(&a, alen, &b, blen) > 0);
		unit_assert(sockaddr_cmp_addr(&b, blen, &a, alen) < 0);
		unit_assert(sockaddr_cmp_addr(&a, alen, &a, alen) == 0);
		unit_assert(ipstrtoaddr("2001:3578:ffeb::99", 53, &b, &blen));
		unit_assert(sockaddr_cmp_addr(&b, blen, &b, blen) == 0);
		unit_assert(sockaddr_cmp_addr(&a, alen, &b, blen) < 0);
		unit_assert(sockaddr_cmp_addr(&b, blen, &a, alen) > 0);
	}
	/* test addr_is_ip4mapped */
	unit_show_func("util/net_help.c", "addr_is_ip4mapped");
	if(1) {
		struct sockaddr_storage a;
		socklen_t l = (socklen_t)sizeof(a);
		unit_assert(ipstrtoaddr("12.13.14.15", 53, &a, &l));
		unit_assert(!addr_is_ip4mapped(&a, l));
		unit_assert(ipstrtoaddr("fe80::217:31ff:fe91:df", 53, &a, &l));
		unit_assert(!addr_is_ip4mapped(&a, l));
		unit_assert(ipstrtoaddr("ffff::217:31ff:fe91:df", 53, &a, &l));
		unit_assert(!addr_is_ip4mapped(&a, l));
		unit_assert(ipstrtoaddr("::ffff:31ff:fe91:df", 53, &a, &l));
		unit_assert(!addr_is_ip4mapped(&a, l));
		unit_assert(ipstrtoaddr("::fffe:fe91:df", 53, &a, &l));
		unit_assert(!addr_is_ip4mapped(&a, l));
		unit_assert(ipstrtoaddr("::ffff:127.0.0.1", 53, &a, &l));
		unit_assert(addr_is_ip4mapped(&a, l));
		unit_assert(ipstrtoaddr("::ffff:127.0.0.2", 53, &a, &l));
		unit_assert(addr_is_ip4mapped(&a, l));
		unit_assert(ipstrtoaddr("::ffff:192.168.0.2", 53, &a, &l));
		unit_assert(addr_is_ip4mapped(&a, l));
		unit_assert(ipstrtoaddr("2::ffff:192.168.0.2", 53, &a, &l));
		unit_assert(!addr_is_ip4mapped(&a, l));
	}
	/* test addr_is_any */
	unit_show_func("util/net_help.c", "addr_is_any");
	if(1) {
		struct sockaddr_storage a;
		socklen_t l = (socklen_t)sizeof(a);
		unit_assert(ipstrtoaddr("0.0.0.0", 53, &a, &l));
		unit_assert(addr_is_any(&a, l));
		unit_assert(ipstrtoaddr("0.0.0.0", 10053, &a, &l));
		unit_assert(addr_is_any(&a, l));
		unit_assert(ipstrtoaddr("0.0.0.0", 0, &a, &l));
		unit_assert(addr_is_any(&a, l));
		unit_assert(ipstrtoaddr("::0", 0, &a, &l));
		unit_assert(addr_is_any(&a, l));
		unit_assert(ipstrtoaddr("::0", 53, &a, &l));
		unit_assert(addr_is_any(&a, l));
		unit_assert(ipstrtoaddr("::1", 53, &a, &l));
		unit_assert(!addr_is_any(&a, l));
		unit_assert(ipstrtoaddr("2001:1667::1", 0, &a, &l));
		unit_assert(!addr_is_any(&a, l));
		unit_assert(ipstrtoaddr("2001::0", 0, &a, &l));
		unit_assert(!addr_is_any(&a, l));
		unit_assert(ipstrtoaddr("10.0.0.0", 0, &a, &l));
		unit_assert(!addr_is_any(&a, l));
		unit_assert(ipstrtoaddr("0.0.0.10", 0, &a, &l));
		unit_assert(!addr_is_any(&a, l));
		unit_assert(ipstrtoaddr("192.0.2.1", 0, &a, &l));
		unit_assert(!addr_is_any(&a, l));
	}
}

#include "util/config_file.h"
/** test config_file: cfg_parse_memsize */
static void
config_memsize_test(void) 
{
	size_t v = 0;
	unit_show_func("util/config_file.c", "cfg_parse_memsize");
	if(0) {
		/* these emit errors */
		unit_assert( cfg_parse_memsize("", &v) == 0);
		unit_assert( cfg_parse_memsize("bla", &v) == 0);
		unit_assert( cfg_parse_memsize("nop", &v) == 0);
		unit_assert( cfg_parse_memsize("n0b", &v) == 0);
		unit_assert( cfg_parse_memsize("gb", &v) == 0);
		unit_assert( cfg_parse_memsize("b", &v) == 0);
		unit_assert( cfg_parse_memsize("kb", &v) == 0);
		unit_assert( cfg_parse_memsize("kk kb", &v) == 0);
	}
	unit_assert( cfg_parse_memsize("0", &v) && v==0);
	unit_assert( cfg_parse_memsize("1", &v) && v==1);
	unit_assert( cfg_parse_memsize("10", &v) && v==10);
	unit_assert( cfg_parse_memsize("10b", &v) && v==10);
	unit_assert( cfg_parse_memsize("5b", &v) && v==5);
	unit_assert( cfg_parse_memsize("1024", &v) && v==1024);
	unit_assert( cfg_parse_memsize("1k", &v) && v==1024);
	unit_assert( cfg_parse_memsize("1K", &v) && v==1024);
	unit_assert( cfg_parse_memsize("1Kb", &v) && v==1024);
	unit_assert( cfg_parse_memsize("1kb", &v) && v==1024);
	unit_assert( cfg_parse_memsize("1 kb", &v) && v==1024);
	unit_assert( cfg_parse_memsize("10 kb", &v) && v==10240);
	unit_assert( cfg_parse_memsize("2k", &v) && v==2048);
	unit_assert( cfg_parse_memsize("2m", &v) && v==2048*1024);
	unit_assert( cfg_parse_memsize("3M", &v) && v==3072*1024);
	unit_assert( cfg_parse_memsize("40m", &v) && v==40960*1024);
	unit_assert( cfg_parse_memsize("1G", &v) && v==1024*1024*1024);
	unit_assert( cfg_parse_memsize("1 Gb", &v) && v==1024*1024*1024);
	unit_assert( cfg_parse_memsize("0 Gb", &v) && v==0*1024*1024);
}

/** test config_file: test tag code */
static void
config_tag_test(void) 
{
	unit_show_func("util/config_file.c", "taglist_intersect");
	unit_assert( taglist_intersect(
		(uint8_t*)"\000\000\000", 3, (uint8_t*)"\001\000\001", 3
		) == 0);
	unit_assert( taglist_intersect(
		(uint8_t*)"\000\000\001", 3, (uint8_t*)"\001\000\001", 3
		) == 1);
	unit_assert( taglist_intersect(
		(uint8_t*)"\001\000\000", 3, (uint8_t*)"\001\000\001", 3
		) == 1);
	unit_assert( taglist_intersect(
		(uint8_t*)"\001", 1, (uint8_t*)"\001\000\001", 3
		) == 1);
	unit_assert( taglist_intersect(
		(uint8_t*)"\001\000\001", 3, (uint8_t*)"\001", 1
		) == 1);
}
	
#include "util/rtt.h"
#include "util/timehist.h"
#include "iterator/iterator.h"
#include "libunbound/unbound.h"
/** test RTT code */
static void
rtt_test(void)
{
	int init = UNKNOWN_SERVER_NICENESS;
	int i;
	struct rtt_info r;
	unit_show_func("util/rtt.c", "rtt_timeout");
	rtt_init(&r);
	/* initial value sensible */
	unit_assert( rtt_timeout(&r) == init );
	rtt_lost(&r, init);
	unit_assert( rtt_timeout(&r) == init*2 );
	rtt_lost(&r, init*2);
	unit_assert( rtt_timeout(&r) == init*4 );
	rtt_update(&r, 4000);
	unit_assert( rtt_timeout(&r) >= 2000 );
	rtt_lost(&r, rtt_timeout(&r) );
	for(i=0; i<100; i++) {
		rtt_lost(&r, rtt_timeout(&r) ); 
		unit_assert( rtt_timeout(&r) > RTT_MIN_TIMEOUT-1);
		unit_assert( rtt_timeout(&r) < RTT_MAX_TIMEOUT+1);
	}
	/* must be the same, timehist bucket is used in stats */
	unit_assert(UB_STATS_BUCKET_NUM == NUM_BUCKETS_HIST);
}

#include "services/cache/infra.h"

/* lookup and get key and data structs easily */
static struct infra_data* infra_lookup_host(struct infra_cache* infra,
	struct sockaddr_storage* addr, socklen_t addrlen, uint8_t* zone,
	size_t zonelen, int wr, time_t now, struct infra_key** k)
{
	struct infra_data* d;
	struct lruhash_entry* e = infra_lookup_nottl(infra, addr, addrlen,
		zone, zonelen, wr);
	if(!e) return NULL;
	d = (struct infra_data*)e->data;
	if(d->ttl < now) {
		lock_rw_unlock(&e->lock);
		return NULL;
	}
	*k = (struct infra_key*)e->key;
	return d;
}

/** test host cache */
static void
infra_test(void)
{
	struct sockaddr_storage one;
	socklen_t onelen;
	uint8_t* zone = (uint8_t*)"\007example\003com\000";
	size_t zonelen = 13;
	struct infra_cache* slab;
	struct config_file* cfg = config_create();
	time_t now = 0;
	uint8_t edns_lame;
	int vs, to;
	struct infra_key* k;
	struct infra_data* d;
	int init = 376;

	unit_show_feature("infra cache");
	unit_assert(ipstrtoaddr("127.0.0.1", 53, &one, &onelen));

	slab = infra_create(cfg);
	unit_assert( infra_host(slab, &one, onelen, zone, zonelen, now,
		&vs, &edns_lame, &to) );
	unit_assert( vs == 0 && to == init && edns_lame == 0 );

	unit_assert( infra_rtt_update(slab, &one, onelen, zone, zonelen, LDNS_RR_TYPE_A, -1, init, now) );
	unit_assert( infra_host(slab, &one, onelen, zone, zonelen, 
			now, &vs, &edns_lame, &to) );
	unit_assert( vs == 0 && to == init*2 && edns_lame == 0 );

	unit_assert( infra_edns_update(slab, &one, onelen, zone, zonelen, -1, now) );
	unit_assert( infra_host(slab, &one, onelen, zone, zonelen, 
			now, &vs, &edns_lame, &to) );
	unit_assert( vs == -1 && to == init*2  && edns_lame == 1);

	now += cfg->host_ttl + 10;
	unit_assert( infra_host(slab, &one, onelen, zone, zonelen, 
			now, &vs, &edns_lame, &to) );
	unit_assert( vs == 0 && to == init && edns_lame == 0 );
	
	unit_assert( infra_set_lame(slab, &one, onelen,
		zone, zonelen,  now, 0, 0, LDNS_RR_TYPE_A) );
	unit_assert( (d=infra_lookup_host(slab, &one, onelen, zone, zonelen, 0, now, &k)) );
	unit_assert( d->ttl == now+cfg->host_ttl );
	unit_assert( d->edns_version == 0 );
	unit_assert(!d->isdnsseclame && !d->rec_lame && d->lame_type_A &&
		!d->lame_other);
	lock_rw_unlock(&k->entry.lock);

	/* test merge of data */
	unit_assert( infra_set_lame(slab, &one, onelen,
		zone, zonelen,  now, 0, 0, LDNS_RR_TYPE_AAAA) );
	unit_assert( (d=infra_lookup_host(slab, &one, onelen, zone, zonelen, 0, now, &k)) );
	unit_assert(!d->isdnsseclame && !d->rec_lame && d->lame_type_A &&
		d->lame_other);
	lock_rw_unlock(&k->entry.lock);

	/* test that noEDNS cannot overwrite known-yesEDNS */
	now += cfg->host_ttl + 10;
	unit_assert( infra_host(slab, &one, onelen, zone, zonelen, 
			now, &vs, &edns_lame, &to) );
	unit_assert( vs == 0 && to == init && edns_lame == 0 );

	unit_assert( infra_edns_update(slab, &one, onelen, zone, zonelen, 0, now) );
	unit_assert( infra_host(slab, &one, onelen, zone, zonelen, 
			now, &vs, &edns_lame, &to) );
	unit_assert( vs == 0 && to == init && edns_lame == 1 );

	unit_assert( infra_edns_update(slab, &one, onelen, zone, zonelen, -1, now) );
	unit_assert( infra_host(slab, &one, onelen, zone, zonelen, 
			now, &vs, &edns_lame, &to) );
	unit_assert( vs == 0 && to == init && edns_lame == 1 );

	infra_delete(slab);
	config_delete(cfg);
}

#include "util/random.h"
/** test randomness */
static void
rnd_test(void)
{
	struct ub_randstate* r;
	int num = 1000, i;
	long int a[1000];
	unit_show_feature("ub_random");
	unit_assert( (r = ub_initstate(NULL)) );
	for(i=0; i<num; i++) {
		a[i] = ub_random(r);
		unit_assert(a[i] >= 0);
		unit_assert((size_t)a[i] <= (size_t)0x7fffffff);
		if(i > 5)
			unit_assert(a[i] != a[i-1] || a[i] != a[i-2] ||
				a[i] != a[i-3] || a[i] != a[i-4] ||
				a[i] != a[i-5] || a[i] != a[i-6]);
	}
	a[0] = ub_random_max(r, 1);
	unit_assert(a[0] >= 0 && a[0] < 1);
	a[0] = ub_random_max(r, 10000);
	unit_assert(a[0] >= 0 && a[0] < 10000);
	for(i=0; i<num; i++) {
		a[i] = ub_random_max(r, 10);
		unit_assert(a[i] >= 0 && a[i] < 10);
	}
	ub_randfree(r);
}

#include "respip/respip.h"
#include "services/localzone.h"
#include "util/data/packed_rrset.h"
typedef struct addr_action {char* ip; char* sact; enum respip_action act;}
	addr_action_t;

/** Utility function that verifies that the respip set has actions as expected */
static void
verify_respip_set_actions(struct respip_set* set, addr_action_t actions[],
	int actions_len)
{
	int i = 0;
	struct rbtree_type* tree = respip_set_get_tree(set);
	for (i=0; i<actions_len; i++) {
		struct sockaddr_storage addr;
		int net;
		socklen_t addrlen;
		struct resp_addr* node;
		netblockstrtoaddr(actions[i].ip, UNBOUND_DNS_PORT, &addr,
			&addrlen, &net);
		node = (struct resp_addr*)addr_tree_find(tree, &addr, addrlen, net);

		/** we have the node and the node has the correct action
		  * and has no data */
		unit_assert(node);
		unit_assert(actions[i].act ==
			resp_addr_get_action(node));
		unit_assert(resp_addr_get_rrset(node) == NULL);
	}
	unit_assert(actions_len && i == actions_len);
	unit_assert(actions_len == (int)tree->count);
}

/** Global respip actions test; apply raw config data and verify that
  * all the nodes in the respip set, looked up by address, have expected
  * actions */
static void
respip_conf_actions_test(void)
{
	addr_action_t config_response_ip[] = {
		{"192.0.1.0/24", "deny", respip_deny},
		{"192.0.2.0/24", "redirect", respip_redirect},
		{"192.0.3.0/26", "inform", respip_inform},
		{"192.0.4.0/27", "inform_deny", respip_inform_deny},
		{"2001:db8:1::/48", "always_transparent", respip_always_transparent},
		{"2001:db8:2::/49", "always_refuse", respip_always_refuse},
		{"2001:db8:3::/50", "always_nxdomain", respip_always_nxdomain},
	};
	int i;
	struct respip_set* set = respip_set_create();
	struct config_file cfg;
	int clen = (int)(sizeof(config_response_ip) / sizeof(addr_action_t));

	unit_assert(set);
	unit_show_feature("global respip config actions apply");
	memset(&cfg, 0, sizeof(cfg));
	for(i=0; i<clen; i++) {
		char* ip = strdup(config_response_ip[i].ip);
		char* sact = strdup(config_response_ip[i].sact);
		unit_assert(ip && sact);
		if(!cfg_str2list_insert(&cfg.respip_actions, ip, sact))
			unit_assert(0);
	}
	unit_assert(respip_global_apply_cfg(set, &cfg));
	verify_respip_set_actions(set, config_response_ip, clen);

	respip_set_delete(set);
	config_deldblstrlist(cfg.respip_actions);
}

/** Per-view respip actions test; apply raw configuration with two views
  * and verify that actions are as expected in respip sets of both views */
static void
respip_view_conf_actions_test(void)
{
	addr_action_t config_response_ip_view1[] = {
		{"192.0.1.0/24", "deny", respip_deny},
		{"192.0.2.0/24", "redirect", respip_redirect},
		{"192.0.3.0/26", "inform", respip_inform},
		{"192.0.4.0/27", "inform_deny", respip_inform_deny},
	};
	addr_action_t config_response_ip_view2[] = {
		{"2001:db8:1::/48", "always_transparent", respip_always_transparent},
		{"2001:db8:2::/49", "always_refuse", respip_always_refuse},
		{"2001:db8:3::/50", "always_nxdomain", respip_always_nxdomain},
	};
	int i;
	struct config_file cfg;
	int clen1 = (int)(sizeof(config_response_ip_view1) / sizeof(addr_action_t));
	int clen2 = (int)(sizeof(config_response_ip_view2) / sizeof(addr_action_t));
	struct config_view* cv1;
	struct config_view* cv2;
	int have_respip_cfg = 0;
	struct views* views = NULL;
	struct view* v = NULL;

	unit_show_feature("per-view respip config actions apply");
	memset(&cfg, 0, sizeof(cfg));
	cv1 = (struct config_view*)calloc(1, sizeof(struct config_view));
	cv2 = (struct config_view*)calloc(1, sizeof(struct config_view));
	unit_assert(cv1 && cv2);
	cv1->name = strdup("view1");
	cv2->name = strdup("view2");
	unit_assert(cv1->name && cv2->name);
	cv1->next = cv2;
	cfg.views = cv1;

	for(i=0; i<clen1; i++) {
		char* ip = strdup(config_response_ip_view1[i].ip);
		char* sact = strdup(config_response_ip_view1[i].sact);
		unit_assert(ip && sact);
		if(!cfg_str2list_insert(&cv1->respip_actions, ip, sact))
			unit_assert(0);
	}
	for(i=0; i<clen2; i++) {
		char* ip = strdup(config_response_ip_view2[i].ip);
		char* sact = strdup(config_response_ip_view2[i].sact);
		unit_assert(ip && sact);
		if(!cfg_str2list_insert(&cv2->respip_actions, ip, sact))
			unit_assert(0);
	}
	views = views_create();
	unit_assert(views);
	unit_assert(views_apply_cfg(views, &cfg));
	unit_assert(respip_views_apply_cfg(views, &cfg, &have_respip_cfg));

	/* now verify the respip sets in each view */
	v = views_find_view(views, "view1", 0);
	unit_assert(v);
	verify_respip_set_actions(v->respip_set, config_response_ip_view1, clen1);
	lock_rw_unlock(&v->lock);
	v = views_find_view(views, "view2", 0);
	unit_assert(v);
	verify_respip_set_actions(v->respip_set, config_response_ip_view2, clen2);
	lock_rw_unlock(&v->lock);

	views_delete(views);
	free(cv1->name);
	free(cv1);
	free(cv2->name);
	free(cv2);
}

typedef struct addr_data {char* ip; char* data;} addr_data_t;

/** find the respip address node in the specified tree (by address lookup)
  * and verify type and address of the specified rdata (by index) in this
  * node's rrset */
static void
verify_rrset(struct respip_set* set, const char* ipstr,
	const char* rdatastr, size_t rdi, uint16_t type)
{
	struct sockaddr_storage addr;
	int net;
	char buf[65536];
	socklen_t addrlen;
	struct rbtree_type* tree;
	struct resp_addr* node;
	const struct ub_packed_rrset_key* rrs;

	netblockstrtoaddr(ipstr, UNBOUND_DNS_PORT, &addr, &addrlen, &net);
	tree = respip_set_get_tree(set);
	node = (struct resp_addr*)addr_tree_find(tree, &addr, addrlen, net);
	unit_assert(node);
	unit_assert((rrs = resp_addr_get_rrset(node)));
	unit_assert(ntohs(rrs->rk.type) == type);
	packed_rr_to_string((struct ub_packed_rrset_key*)rrs,
		rdi, 0, buf, sizeof(buf));
	unit_assert(strstr(buf, rdatastr));
}

/** Dataset used to test redirect rrset initialization for both
  * global and per-view respip redirect configuration */
static addr_data_t config_response_ip_data[] = {
	{"192.0.1.0/24", "A 1.2.3.4"},
	{"192.0.1.0/24", "A 11.12.13.14"},
	{"192.0.2.0/24", "CNAME www.example.com."},
	{"2001:db8:1::/48", "AAAA 2001:db8:1::2:1"},
};

/** Populate raw respip redirect config data, used for both global and
  * view-based respip redirect test case */
static void
cfg_insert_respip_data(struct config_str2list** respip_actions,
	struct config_str2list** respip_data)
{
	int clen = (int)(sizeof(config_response_ip_data) / sizeof(addr_data_t));
	int i = 0;

	/* insert actions (duplicate netblocks don't matter) */
	for(i=0; i<clen; i++) {
		char* ip = strdup(config_response_ip_data[i].ip);
		char* sact = strdup("redirect");
		unit_assert(ip && sact);
		if(!cfg_str2list_insert(respip_actions, ip, sact))
			unit_assert(0);
	}
	/* insert data */
	for(i=0; i<clen; i++) {
		char* ip = strdup(config_response_ip_data[i].ip);
		char* data = strdup(config_response_ip_data[i].data);
		unit_assert(ip && data);
		if(!cfg_str2list_insert(respip_data, ip, data))
			unit_assert(0);
	}
}

/** Test global respip redirect w/ data directives */
static void
respip_conf_data_test(void)
{
	struct respip_set* set = respip_set_create();
	struct config_file cfg;

	unit_show_feature("global respip config data apply");
	memset(&cfg, 0, sizeof(cfg));

	cfg_insert_respip_data(&cfg.respip_actions, &cfg.respip_data);

	/* apply configuration and verify rrsets */
	unit_assert(respip_global_apply_cfg(set, &cfg));
	verify_rrset(set, "192.0.1.0/24", "1.2.3.4", 0, LDNS_RR_TYPE_A);
	verify_rrset(set, "192.0.1.0/24", "11.12.13.14", 1, LDNS_RR_TYPE_A);
	verify_rrset(set, "192.0.2.0/24", "www.example.com", 0, LDNS_RR_TYPE_CNAME);
	verify_rrset(set, "2001:db8:1::/48", "2001:db8:1::2:1", 0, LDNS_RR_TYPE_AAAA);

	respip_set_delete(set);
}

/** Test per-view respip redirect w/ data directives */
static void
respip_view_conf_data_test(void)
{
	struct config_file cfg;
	struct config_view* cv;
	int have_respip_cfg = 0;
	struct views* views = NULL;
	struct view* v = NULL;

	unit_show_feature("per-view respip config data apply");
	memset(&cfg, 0, sizeof(cfg));
	cv = (struct config_view*)calloc(1, sizeof(struct config_view));
	unit_assert(cv);
	cv->name = strdup("view1");
	unit_assert(cv->name);
	cfg.views = cv;
	cfg_insert_respip_data(&cv->respip_actions, &cv->respip_data);
	views = views_create();
	unit_assert(views);
	unit_assert(views_apply_cfg(views, &cfg));

	/* apply configuration and verify rrsets */
	unit_assert(respip_views_apply_cfg(views, &cfg, &have_respip_cfg));
	v = views_find_view(views, "view1", 0);
	unit_assert(v);
	verify_rrset(v->respip_set, "192.0.1.0/24", "1.2.3.4",
		0, LDNS_RR_TYPE_A);
	verify_rrset(v->respip_set, "192.0.1.0/24", "11.12.13.14",
		1, LDNS_RR_TYPE_A);
	verify_rrset(v->respip_set, "192.0.2.0/24", "www.example.com",
		0, LDNS_RR_TYPE_CNAME);
	verify_rrset(v->respip_set, "2001:db8:1::/48", "2001:db8:1::2:1",
		0, LDNS_RR_TYPE_AAAA);
	lock_rw_unlock(&v->lock);

	views_delete(views);
	free(cv->name);
	free(cv);
}

/** respip unit tests */
static void respip_test(void)
{
	respip_view_conf_data_test();
	respip_conf_data_test();
	respip_view_conf_actions_test();
	respip_conf_actions_test();
}

#include <ctype.h>
#include "sldns/str2wire.h"
#include "services/authzone.h"
#include "util/data/dname.h"
#include "util/regional.h"
#include "validator/val_anchor.h"
/** Add zone from file for testing */
struct auth_zone* authtest_addzone(struct auth_zones* az, const char* name,
	char* fname);

/** zonemd unit test, generate a zonemd digest and check if correct */
static void zonemd_generate_test(const char* zname, char* zfile,
	int scheme, int hashalgo, const char* digest)
{
	uint8_t zonemd_hash[512];
	size_t hashlen = 0;
	char output[1024+1];
	size_t i;
	struct auth_zones* az;
	struct auth_zone* z;
	int result;
	struct regional* region = NULL;
	struct sldns_buffer* buf = NULL;
	char* reason = NULL;
	char* digestdup;

	if(!zonemd_hashalgo_supported(hashalgo))
		return; /* cannot test unsupported algo */

	/* setup environment */
	az = auth_zones_create();
	unit_assert(az);
	region = regional_create();
	unit_assert(region);
	buf = sldns_buffer_new(65535);
	unit_assert(buf);

	/* read file */
	z = authtest_addzone(az, zname, zfile);
	unit_assert(z);

	/* create zonemd digest */
	result = auth_zone_generate_zonemd_hash(z, scheme, hashalgo,
		zonemd_hash, sizeof(zonemd_hash), &hashlen, region, buf,
		&reason);
	if(reason) printf("zonemd failure reason: %s\n", reason);
	unit_assert(result);

	/* check digest */
	unit_assert(hashlen*2+1 <= sizeof(output));
	for(i=0; i<hashlen; i++) {
		const char* hexl = "0123456789ABCDEF";
		output[i*2] = hexl[(zonemd_hash[i]&0xf0)>>4];
		output[i*2+1] = hexl[zonemd_hash[i]&0xf];
	}
	output[hashlen*2] = 0;
	digestdup = strdup(digest);
	unit_assert(digestdup);
	for(i=0; i<strlen(digestdup); i++) {
		digestdup[i] = toupper(digestdup[i]);
	}
	if(0) {
		char zname[255+1];
		dname_str(z->name, zname);
		printf("zonemd generated for %s in %s with "
			"scheme=%d hashalgo=%d\n", zname, z->zonefile,
			scheme, hashalgo);
		printf("digest %s\n", output);
		printf("wanted %s\n", digestdup);
	}
	unit_assert(strcmp(output, digestdup) == 0);

	/* delete environment */
	free(digestdup);
	auth_zones_delete(az);
	regional_destroy(region);
	sldns_buffer_free(buf);
}

/** loop over files and test generated zonemd digest */
static void zonemd_generate_tests(void)
{
	unit_show_func("services/authzone.c", "auth_zone_generate_zonemd_hash");
	zonemd_generate_test("example.org", "testdata/zonemd.example1.zone",
		1, 2, "20564D10F50A0CEBEC856C64032B7DFB53D3C449A421A5BC7A21F7627B4ACEA4DF29F2C6FE82ED9C23ADF6F4D420D5DD63EF6E6349D60FDAB910B65DF8D481B7");

	/* https://tools.ietf.org/html/draft-ietf-dnsop-dns-zone-digest-12
	 * from section A.1 */
	zonemd_generate_test("example", "testdata/zonemd.example_a1.zone",
		1, 1, "c68090d90a7aed716bc459f9340e3d7c1370d4d24b7e2fc3a1ddc0b9a87153b9a9713b3c9ae5cc27777f98b8e730044c");

	/* https://tools.ietf.org/html/draft-ietf-dnsop-dns-zone-digest-12
	 * from section A.2 */
	zonemd_generate_test("example", "testdata/zonemd.example_a2.zone",
		1, 1, "31cefb03814f5062ad12fa951ba0ef5f8da6ae354a415767246f7dc932ceb1e742a2108f529db6a33a11c01493de358d");

	/* https://tools.ietf.org/html/draft-ietf-dnsop-dns-zone-digest-12
	 * from section A.3 SHA384 digest */
	zonemd_generate_test("example", "testdata/zonemd.example_a3.zone",
		1, 1, "62e6cf51b02e54b9b5f967d547ce43136792901f9f88e637493daaf401c92c279dd10f0edb1c56f8080211f8480ee306");

	/* https://tools.ietf.org/html/draft-ietf-dnsop-dns-zone-digest-12
	 * from section A.3 SHA512 digest*/
	zonemd_generate_test("example", "testdata/zonemd.example_a3.zone",
		1, 2, "08cfa1115c7b948c4163a901270395ea226a930cd2cbcf2fa9a5e6eb85f37c8a4e114d884e66f176eab121cb02db7d652e0cc4827e7a3204f166b47e5613fd27");

	/* https://tools.ietf.org/html/draft-ietf-dnsop-dns-zone-digest-12
	 * from section A.4 */
	zonemd_generate_test("uri.arpa", "testdata/zonemd.example_a4.zone",
		1, 1, "1291b78ddf7669b1a39d014d87626b709b55774c5d7d58fadc556439889a10eaf6f11d615900a4f996bd46279514e473");

	/* https://tools.ietf.org/html/draft-ietf-dnsop-dns-zone-digest-12
	 * from section A.5 */
	zonemd_generate_test("root-servers.net", "testdata/zonemd.example_a5.zone",
		1, 1, "f1ca0ccd91bd5573d9f431c00ee0101b2545c97602be0a978a3b11dbfc1c776d5b3e86ae3d973d6b5349ba7f04340f79");
}

/** test the zonemd check routine */
static void zonemd_check_test(void)
{
	const char* zname = "example.org";
	char* zfile = "testdata/zonemd.example1.zone";
	int scheme = 1;
	int hashalgo = 2;
	const char* digest = "20564D10F50A0CEBEC856C64032B7DFB53D3C449A421A5BC7A21F7627B4ACEA4DF29F2C6FE82ED9C23ADF6F4D420D5DD63EF6E6349D60FDAB910B65DF8D481B7";
	const char* digestwrong = "20564D10F50A0CEBEC856C64032B7DFB53D3C449A421A5BC7A21F7627B4ACEA4DF29F2C6FE82ED9C23ADF6F4D420D5DD63EF6E6349D60FDAB910B65DF8D48100";
	uint8_t hash[512], hashwrong[512];
	size_t hashlen = 0, hashwronglen = 0;
	struct auth_zones* az;
	struct auth_zone* z;
	int result;
	struct regional* region = NULL;
	struct sldns_buffer* buf = NULL;
	char* reason = NULL;

	if(!zonemd_hashalgo_supported(hashalgo))
		return; /* cannot test unsupported algo */
	unit_show_func("services/authzone.c", "auth_zone_generate_zonemd_check");

	/* setup environment */
	az = auth_zones_create();
	unit_assert(az);
	region = regional_create();
	unit_assert(region);
	buf = sldns_buffer_new(65535);
	unit_assert(buf);

	/* read file */
	z = authtest_addzone(az, zname, zfile);
	unit_assert(z);
	hashlen = sizeof(hash);
	if(sldns_str2wire_hex_buf(digest, hash, &hashlen) != 0) {
		unit_assert(0); /* parse failure */
	}
	hashwronglen = sizeof(hashwrong);
	if(sldns_str2wire_hex_buf(digestwrong, hashwrong, &hashwronglen) != 0) {
		unit_assert(0); /* parse failure */
	}

	/* check return values of the check routine */
	result = auth_zone_generate_zonemd_check(z, scheme, hashalgo,
		hash, hashlen, region, buf, &reason);
	unit_assert(result && reason == NULL);
	result = auth_zone_generate_zonemd_check(z, 241, hashalgo,
		hash, hashlen, region, buf, &reason);
	unit_assert(!result && strcmp(reason, "unsupported scheme")==0);
	result = auth_zone_generate_zonemd_check(z, scheme, 242,
		hash, hashlen, region, buf, &reason);
	unit_assert(!result && strcmp(reason, "unsupported algorithm")==0);
	result = auth_zone_generate_zonemd_check(z, scheme, hashalgo,
		hash, 2, region, buf, &reason);
	unit_assert(!result && strcmp(reason, "digest length too small, less than 12")==0);
	result = auth_zone_generate_zonemd_check(z, scheme, hashalgo,
		hashwrong, hashwronglen, region, buf, &reason);
	unit_assert(!result && strcmp(reason, "incorrect digest")==0);
	result = auth_zone_generate_zonemd_check(z, scheme, hashalgo,
		hashwrong, hashwronglen-3, region, buf, &reason);
	unit_assert(!result && strcmp(reason, "incorrect digest length")==0);

	/* delete environment */
	auth_zones_delete(az);
	regional_destroy(region);
	sldns_buffer_free(buf);
}

/** zonemd test verify */
static void zonemd_verify_test(char* zname, char* zfile, char* tastr,
	char* date_override, char* result_wanted)
{
	struct module_stack mods;
	struct module_env env;
	char* result = NULL;
	struct auth_zone* z;

	/* setup test harness */
	memset(&mods, 0, sizeof(mods));
	memset(&env, 0, sizeof(env));
	env.scratch = regional_create();
	if(!env.scratch)
		fatal_exit("out of memory");
	env.scratch_buffer = sldns_buffer_new(65553);
	if(!env.scratch_buffer)
		fatal_exit("out of memory");
	env.cfg = config_create();
	if(!env.cfg)
		fatal_exit("out of memory");
	env.cfg->val_date_override = cfg_convert_timeval(date_override);
	if(!env.cfg->val_date_override)
		fatal_exit("could not parse datetime %s", date_override);
	env.anchors = anchors_create();
	if(!env.anchors)
		fatal_exit("out of memory");
	env.auth_zones = auth_zones_create();
	if(!env.auth_zones)
		fatal_exit("out of memory");
	modstack_init(&mods);
	if(!modstack_config(&mods, "validator iterator"))
		fatal_exit("could not init modules");
	env.mesh = mesh_create(&mods, &env);
	if(!env.mesh)
		fatal_exit("out of memory");

	/* load data */
	if(tastr && !anchor_store_str(env.anchors, env.scratch_buffer, tastr))
		fatal_exit("could not store anchor: %s", tastr);
	z = authtest_addzone(env.auth_zones, zname, zfile);
	if(!z)
		fatal_exit("could not addzone %s %s", zname, zfile);

	/* test */
	lock_rw_wrlock(&z->lock);
	auth_zone_verify_zonemd(z, &env, &result);
	lock_rw_unlock(&z->lock);
	if(0) {
		printf("auth zone %s: ZONEMD verification %s: %s\n", zname,
			(strcmp(result, "ZONEMD verification successful")==0?"successful":"failed"),
			result);
	}
	if(!result)
		fatal_exit("out of memory");
	unit_assert(strcmp(result, result_wanted) == 0);
	if(strcmp(result, "ZONEMD verification successful") == 0) {
		lock_rw_rdlock(&z->lock);
		unit_assert(!z->zone_expired);
		lock_rw_unlock(&z->lock);
	} else {
		lock_rw_rdlock(&z->lock);
		unit_assert(z->zone_expired);
		lock_rw_unlock(&z->lock);
	}
	free(result);

	/* desetup test harness */
	mesh_delete(env.mesh);
	modstack_desetup(&mods, &env);
	auth_zones_delete(env.auth_zones);
	anchors_delete(env.anchors);
	config_delete(env.cfg);
	regional_destroy(env.scratch);
	sldns_buffer_free(env.scratch_buffer);
}

/** zonemd test verify suite */
static void zonemd_verify_tests(void)
{
	unit_show_func("services/authzone.c", "auth_zone_verify_zonemd");
	zonemd_verify_test("example.org",
		"testdata/zonemd.example1.zone",
		"example.org. IN DS 55566 8 2 9c148338951ce1c3b5cd3da532f3d90dfcf92595148022f2c2fd98e5deee90af",
		"20180302005009",
		"have trust anchor, but zone has no DNSKEY");
	zonemd_verify_test("example.org",
		"testdata/zonemd.example1.zone",
		NULL,
		"20180302005009",
		"zone has no ZONEMD");
	/* no trust anchor, so it succeeds */
	zonemd_verify_test("example.com",
		"testdata/zonemd.example2.zone",
		NULL,
		"20180302005009",
		"ZONEMD verification successful");
	/* trust anchor for another zone, so it is indeterminate */
	zonemd_verify_test("example.com",
		"testdata/zonemd.example2.zone",
		"example.org. IN DS 55566 8 2 9c148338951ce1c3b5cd3da532f3d90dfcf92595148022f2c2fd98e5deee90af",
		"20180302005009",
		"ZONEMD verification successful");

	/* load a DNSSEC signed zone, but no trust anchor */
	zonemd_verify_test("example.com",
		"testdata/zonemd.example3.zone",
		NULL,
		"20180302005009",
		"incorrect digest");
	/* load a DNSSEC zone with NSEC3, but no trust anchor */
	zonemd_verify_test("example.com",
		"testdata/zonemd.example4.zone",
		NULL,
		"20180302005009",
		"incorrect digest");
	/* valid zonemd, in dnssec signed zone, no trust anchor*/
	zonemd_verify_test("example.com",
		"testdata/zonemd.example5.zone",
		NULL,
		"20180302005009",
		"ZONEMD verification successful");
	/* valid zonemd, in dnssec NSEC3 zone, no trust anchor*/
	zonemd_verify_test("example.com",
		"testdata/zonemd.example6.zone",
		NULL,
		"20180302005009",
		"ZONEMD verification successful");

#if 0
	/* load a DNSSEC signed zone with a trust anchor, valid ZONEMD */
	zonemd_verify_test("example.com",
		"testdata/zonemd.example5.zone",
		"example.com. IN DS 55566 8 2 9c148338951ce1c3b5cd3da532f3d90dfcf92595148022f2c2fd98e5deee90af",
		"20201020135527",
		"ZONEMD verification successful");
	/* load a DNSSEC NSEC3 signed zone with a trust anchor, valid ZONEMD */
	zonemd_verify_test("example.com",
		"testdata/zonemd.example6.zone",
		"example.com. IN DS 55566 8 2 9c148338951ce1c3b5cd3da532f3d90dfcf92595148022f2c2fd98e5deee90af",
		"20201020135527",
		"ZONEMD verification successful");
#endif
}

/** zonemd unit tests */
static void zonemd_test(void)
{
	unit_show_feature("zonemd");
	zonemd_generate_tests();
	zonemd_check_test();
	zonemd_verify_tests();
}

void unit_show_func(const char* file, const char* func)
{
	printf("test %s:%s\n", file, func);
}

void unit_show_feature(const char* feature)
{
	printf("test %s functions\n", feature);
}

#ifdef USE_ECDSA_EVP_WORKAROUND
void ecdsa_evp_workaround_init(void);
#endif
/**
 * Main unit test program. Setup, teardown and report errors.
 * @param argc: arg count.
 * @param argv: array of commandline arguments.
 * @return program failure if test fails.
 */
int 
main(int argc, char* argv[])
{
	log_init(NULL, 0, NULL);
	if(argc != 1) {
		printf("usage: %s\n", argv[0]);
		printf("\tperforms unit tests.\n");
		return 1;
	}
	/* Disable roundrobin for the unit tests */
	RRSET_ROUNDROBIN = 0;
#ifdef USE_LIBEVENT
	printf("Start of %s+libevent unit test.\n", PACKAGE_STRING);
#else
	printf("Start of %s unit test.\n", PACKAGE_STRING);
#endif
#ifdef HAVE_SSL
#  ifdef HAVE_ERR_LOAD_CRYPTO_STRINGS
	ERR_load_crypto_strings();
#  endif
#  ifdef USE_GOST
	(void)sldns_key_EVP_load_gost_id();
#  endif
#  ifdef USE_ECDSA_EVP_WORKAROUND
	ecdsa_evp_workaround_init();
#  endif
#elif defined(HAVE_NSS)
	if(NSS_NoDB_Init(".") != SECSuccess)
		fatal_exit("could not init NSS");
#endif /* HAVE_SSL or HAVE_NSS*/
	checklock_start();
	authzone_test();
	neg_test();
	rnd_test();
	respip_test();
	verify_test();
	net_test();
	config_memsize_test();
	config_tag_test();
	dname_test();
	rtt_test();
	anchors_test();
	alloc_test();
	regional_test();
	lruhash_test();
	slabhash_test();
	infra_test();
	ldns_test();
	zonemd_test();
	msgparse_test();
#ifdef CLIENT_SUBNET
	ecs_test();
#endif /* CLIENT_SUBNET */
	if(log_get_lock()) {
		lock_basic_destroy((lock_basic_type*)log_get_lock());
	}
	checklock_stop();
	printf("%d checks ok.\n", testcount);
#ifdef HAVE_SSL
#  if defined(USE_GOST) && defined(HAVE_LDNS_KEY_EVP_UNLOAD_GOST)
	sldns_key_EVP_unload_gost();
#  endif
#  ifdef HAVE_OPENSSL_CONFIG
#  ifdef HAVE_EVP_CLEANUP
	EVP_cleanup();
#  endif
#  if (OPENSSL_VERSION_NUMBER < 0x10100000) && !defined(OPENSSL_NO_ENGINE) && defined(HAVE_ENGINE_CLEANUP)
	ENGINE_cleanup();
#  endif
	CONF_modules_free();
#  endif
#  ifdef HAVE_CRYPTO_CLEANUP_ALL_EX_DATA
	CRYPTO_cleanup_all_ex_data();
#  endif
#  ifdef HAVE_ERR_FREE_STRINGS
	ERR_free_strings();
#  endif
#  ifdef HAVE_RAND_CLEANUP
	RAND_cleanup();
#  endif
#elif defined(HAVE_NSS)
	if(NSS_Shutdown() != SECSuccess)
		fatal_exit("could not shutdown NSS");
#endif /* HAVE_SSL or HAVE_NSS */
#ifdef HAVE_PTHREAD
	/* dlopen frees its thread specific state */
	pthread_exit(NULL);
#endif
	return 0;
}
