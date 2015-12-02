#include <iostream>
#include <iomanip>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#undef main // removes SDLs evil define

#include "../shared/UniversalPacket.h"
#include "../shared/JoinRequestPacket.h"
#include "../shared/JoinResponsePacket.h"
#include "../shared/MapResponsePacket.h"
#include "../shared/MovePacket.h"

#define SERVERIP "127.0.0.1"
#define SERVERPORT 1177
#define MAX_CLIENTS 8

#define MAP_WIDTH 24
#define MAP_HEGIHT 24
char mapData[MAP_WIDTH * MAP_HEGIHT] =
{
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1,
	1, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 1,
	1, 3, 3, 3, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 4, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

struct Client {
	TCPsocket tcp_sock;
	UDPsocket udp_sock;
	int ID;
};

std::vector<Client> clients;

IPaddress address;
TCPsocket TCP_socket;
UDPsocket UDP_socket;
UDPpacket UDP_packet;
UniversalPacket uniPacket;

void init();
void check_for_new_tcp( TCPsocket server_sock );

int main(int argc, char* argv[])
{
	std::cout << "Starting wolf_online server..." << std::endl;

	init();
			

	// create a listening TCP socket for the server
	//TCPsocket TCP_client_sock = NULL;
		

	//check_for_new_tcp( TCP_socket );

	/* Main loop */
	bool quit = false;
	while( !quit )
	{
		/* Wait a packet. UDP_Recv returns != 0 if a packet is coming */
		//SDLNet_UDP_Recv( UDP_socket, &UDP_packet );
		while( SDLNet_UDP_Recv( UDP_socket, &UDP_packet ) )
		{
			/*/ print packet info
			std::cout << "UDP Packet incoming\n" << std::endl;
			std::cout << "\tChan:    " << UDP_packet.channel << std::endl;
			std::cout << "\tLen:     " << UDP_packet.len << std::endl;
			std::cout << "\tMaxlen:  " << UDP_packet.maxlen << std::endl;
			std::cout << "\tStatus:  " << UDP_packet.status << std::endl;
			std::cout << "\tAddress: " << std::hex
				<< UDP_packet.address.host << " " << std::dec
				<< UDP_packet.address.port << std::endl;
			//*/
			auto recvd = uniPacket.CreateFromContents();

			if( recvd )
			{
				if( recvd->Type() == PT_MOVE )
				{
					MovePacket* packet = (MovePacket*)recvd.get();

					std::cout << "ID: " << (int)packet->GetID()
						<< " x: " << packet->GetPosition().x
						<< " y: " << packet->GetPosition().y << std::endl;

					// change the packet's id and send it back
					UDP_packet.data[1] = 118;
					SDLNet_UDP_Send( UDP_socket, -1, &UDP_packet );

				}
				else
				{
					std::cout << "UDP type not recognised" << std::endl;
				}
			}
			else
			{
				//std::cout << "UDP packet not supported" << std::endl;
			}
		}

		//if( clients.size() == 0 )
		//	check_for_new_tcp( TCP_socket );

		// try to accept a connection if not already accepted
		//if( TCP_client_sock == NULL )
		//{
		//	TCP_client_sock = SDLNet_TCP_Accept( TCP_socket );
		//}

		// if a connection has been made, check it
		//if( TCP_client_sock )
		for( Client& client : clients )
		{
			if( client.tcp_sock == NULL )
				continue;
						
			if( SDLNet_TCP_Recv( client.tcp_sock, uniPacket.Data(), uniPacket.Size() ) <= 0 )
			{
				// an error occured, set the socket to null and try connecting again
				// TODO: find out if this leaves allocated memory?
				// TODO: remove the client from the array
				client.tcp_sock = NULL;
				continue;
			}

			auto recvd = uniPacket.CreateFromContents();

			// check if its a join request
			if (recvd)
			{
				//if (recvd->Type() == PT_JOIN_REQUEST)
				//{
				//	std::cout << "player requested join" << std::endl;
				//
				//	JoinResponsePacket response;
				//	response.SetResponse(JR_OK);
				//	response.SetGivenID(117);
				//
				//	SDLNet_TCP_Send( client.tcp_sock, response.Data( ), response.Size( ) );
				//}
				if (recvd->Type() == PT_MAP_REQUEST)
				{
					std::cout << "player asked for map data" << std::endl;

					MapResponsePacket response;
					response.SetMapData(mapData);

					SDLNet_TCP_Send( client.tcp_sock, response.Data( ), response.Size( ) );
				}
				else if( recvd->Type() == PT_MOVE )
				{
					MovePacket* packet = (MovePacket*)recvd.get();

					std::cout << "x: " << packet->GetPosition().x << " y: " << packet->GetPosition().y << std::endl;
				}
				else
				{
					std::cout << "TCP type not recognised" << std::endl;
				}
			}
			else
			{
				std::cout << "TCP packet not supported" << std::endl;
			}
		}
	}

	// cleanup
	SDLNet_Quit( );
	SDL_Quit( );
	return 0;
}

void init()
{
	// Initalise SDL
	if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
	{
		std::cout << "SDL_Init: %s\n" << SDL_GetError( ) << std::endl;
		exit( 1 );
	}
	// Initalise SDLNet
	if( SDLNet_Init( ) == -1 )
	{
		std::cout << "SDLNet_Init: %s\n" << SDLNet_GetError( ) << std::endl;
		exit( 2 );
	}
	// Resolve Host
	if( SDLNet_ResolveHost( &address, NULL, SERVERPORT ) == -1 ) {
		printf( "SDLNet_ResolveHost: %s\n", SDLNet_GetError( ) );
		exit( 3 );
	}
	// Open TCP socket
	TCP_socket = SDLNet_TCP_Open( &address );
	if( !TCP_socket ) {
		printf( "SDLNet_TCP_Open: %s\n", SDLNet_GetError( ) );
		exit( 4 );
	}
	// Open UDP socket
	if( !(UDP_socket = SDLNet_UDP_Open( SERVERPORT )) )
	{
		fprintf( stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError( ) );
		exit( 5 );
	}

	std::cout << "Server listening on port: " << SERVERPORT << std::endl;

	// Intertwine the SDLNet packet and the UniversalPacket
	// This allows us to use the SDL funcitons with custom data handling
	UDP_packet.data = uniPacket.Data( );
	UDP_packet.maxlen = uniPacket.Size( );
}

void check_for_new_tcp(TCPsocket server_sock)
{
	TCPsocket new_socket;

	// try to accept a new connection
	new_socket = SDLNet_TCP_Accept(server_sock);

	// if there was no connection accept returns null
	if (new_socket)
	{
		JoinRequestPacket joinRequest;

		// check for a join request
		if (SDLNet_TCP_Recv(new_socket, joinRequest.Data(), joinRequest.Size()) <= 0)
		{
			// an error occured, set the socket to null and try connecting again
			// TODO: find out if this leaves allocated memory?
			return;
		}

		// if there is space on the server
		if (clients.size() < MAX_CLIENTS)
		{
			// respon with OK
			JoinResponsePacket response;
			response.SetResponse(JR_OK);

			// TODO: make ID allocation not broken
			response.SetGivenID( clients.size() );

			if( SDLNet_TCP_Send( new_socket, response.Data(), response.Size() ) < response.Size() )
			{
				// the whole packet could not be sent, some error occured
				return;
			}

			// add the new client to the vector
			clients.push_back(Client());

			// find an avalible id and reply ok
			clients.back().ID = response.GetGivenID();

			// remember the socket
			clients.back().tcp_sock = new_socket;
		}
		else
		{
			// else reply reject
			JoinResponsePacket response;
			response.SetResponse( JR_REJECT );

			if( SDLNet_TCP_Send( new_socket, response.Data(), response.Size() ) < response.Size() )
			{
				// the whole packet could not be sent, some error occured
				return;
			}
		}
	}
}