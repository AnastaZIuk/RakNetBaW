#include "EmptyHeader.h"

#ifdef RAKNET_SOCKET_2_INLINE_FUNCTIONS

#ifndef RAKNETSOCKET2_WINDOWS_LINUX_CPP
#define RAKNETSOCKET2_WINDOWS_LINUX_CPP

#if !defined(WINDOWS_STORE_RT) && !defined(__native_client__)

#if RAKNET_SUPPORT_IPV6==1

void PrepareAddrInfoHints2(addrinfo *hints)
{
	memset(hints, 0, sizeof (addrinfo)); // make sure the struct is empty
	hints->ai_socktype = SOCK_DGRAM; // UDP sockets
	hints->ai_flags = AI_PASSIVE;     // fill in my IP for me
}

void GetMyIP_Windows_Linux_IPV4And6( SystemAddress addresses[MAXIMUM_NUMBER_OF_INTERNAL_IDS] )
{
	int idx=0;

#if (defined(__GNUC__)  || defined(__GCCXML__)) && !defined(__WIN32__)
    struct ifaddrs *pia = NULL;
    getifaddrs(&pia);

	if ( pia == 0 )
	{
		RakAssert(pia!=0);
		return ;
	}
    struct ifaddrs *orig_pia = pia;
	for ( idx = 0; idx < MAXIMUM_NUMBER_OF_INTERNAL_IDS&&pia; )
	{
	    if (pia->ifa_addr)
        {
            if (pia->ifa_addr->sa_family==AF_INET)
            {
                memcpy(&addresses[idx].address.addr4,&((struct sockaddr_in *)pia->ifa_addr,sizeof(struct sockaddr_in));
                ++idx;
            }
            else if (pia->ifa_addr->sa_family==AF_INET6)
            {
                memcpy(&addresses[idx].address.addr4,&((struct sockaddr_in6 *)pia->ifa_addr,sizeof(struct sockaddr_in6));
                ++idx;
            }
        }

		pia = pia->ifa_next;
	}
    freeifaddrs(orig_pia);
#else
	char ac[ 80 ];
	int err = gethostname( ac, sizeof( ac ) );
	RakAssert(err != -1);

	struct addrinfo hints;
	struct addrinfo *servinfo=0, *aip;  // will point to the results
	PrepareAddrInfoHints2(&hints);
	getaddrinfo(ac, "", &hints, &servinfo);

	for (idx=0, aip = servinfo; aip != NULL && idx < MAXIMUM_NUMBER_OF_INTERNAL_IDS; aip = aip->ai_next, idx++)
	{
		if (aip->ai_family == AF_INET)
		{
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)aip->ai_addr;
			memcpy(&addresses[idx].address.addr4,ipv4,sizeof(sockaddr_in));
		}
		else
		{
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)aip->ai_addr;
			memcpy(&addresses[idx].address.addr4,ipv6,sizeof(sockaddr_in6));
		}

	}

	freeaddrinfo(servinfo); // free the linked-list
#endif

	while (idx < MAXIMUM_NUMBER_OF_INTERNAL_IDS)
	{
		addresses[idx]=UNASSIGNED_SYSTEM_ADDRESS;
		idx++;
	}
}

#else

#if (defined(__GNUC__)  || defined(__GCCXML__)) && !defined(__WIN32__)
#include <netdb.h>
#include <unistd.h>
#endif
void GetMyIP_Windows_Linux_IPV4( SystemAddress addresses[MAXIMUM_NUMBER_OF_INTERNAL_IDS] )
{
	int idx=0;

#if (defined(__GNUC__)  || defined(__GCCXML__)) && !defined(__WIN32__)
    struct ifaddrs *pia = NULL;
    getifaddrs(&pia);

	if ( pia == 0 )
	{
		RakAssert(pia!=0);
		return ;
	}
    struct ifaddrs *orig_pia = pia;
	for ( idx = 0; idx < MAXIMUM_NUMBER_OF_INTERNAL_IDS&&pia; )
	{
	    if (pia->ifa_addr)
        {
            if (pia->ifa_addr->sa_family==AF_INET)
            {
                memcpy(&addresses[idx].address.addr4.sin_addr,&((struct sockaddr_in *)pia->ifa_addr)->sin_addr,sizeof(struct in_addr));
                ++idx;
            }
        }

		pia = pia->ifa_next;
	}
    freeifaddrs(orig_pia);
#else
	char ac[ 80 ];
	int err = gethostname( ac, sizeof( ac ) );
    (void) err;
	RakAssert(err != -1);

	struct hostent *phe = gethostbyname( ac );

	if ( phe == 0 )
	{
		RakAssert(phe!=0);
		return ;
	}
	for ( idx = 0; idx < MAXIMUM_NUMBER_OF_INTERNAL_IDS; ++idx )
	{
		if (phe->h_addr_list[ idx ] == 0)
			break;

		memcpy(&addresses[idx].address.addr4.sin_addr,phe->h_addr_list[ idx ],sizeof(struct in_addr));
	}
#endif


	while (idx < MAXIMUM_NUMBER_OF_INTERNAL_IDS)
	{
		addresses[idx]=UNASSIGNED_SYSTEM_ADDRESS;
		idx++;
	}
}

#endif // RAKNET_SUPPORT_IPV6==1


void GetMyIP_Windows_Linux( SystemAddress addresses[MAXIMUM_NUMBER_OF_INTERNAL_IDS] )
{
	#if RAKNET_SUPPORT_IPV6==1
		GetMyIP_Windows_Linux_IPV4And6(addresses);
	#else
		GetMyIP_Windows_Linux_IPV4(addresses);
	#endif
}


#endif // Windows and Linux

#endif // file header

#endif // #ifdef RAKNET_SOCKET_2_INLINE_FUNCTIONS
