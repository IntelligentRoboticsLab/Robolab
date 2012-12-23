/* 
 * Program that can read data from the Robocup GameController
 *
 * @author Steven Laan
 */

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "RoboCupGameControlData.h"

#define BUF_SIZE 512

void fillGCData( char buf[], struct RoboCupGameControlData * GCData );

int main( int argc, char *argv[] ) {
  struct addrinfo hints;
  struct addrinfo *result, *rp;

  struct RoboCupGameControlData GCData;

  int sfd, s;
  size_t len;
  ssize_t nread;
  char buf[BUF_SIZE];

  if ( argc < 2 ) {
     fprintf( stderr, "Usage: %s host port\n", argv[0] );
     exit( EXIT_FAILURE );
  }

  // Fill the hints
  memset( &hints, 0, sizeof( struct addrinfo ) );
  hints.ai_family = AF_UNSPEC;    
  hints.ai_socktype = SOCK_DGRAM; 
  hints.ai_flags = 0;
  hints.ai_protocol = 0;          

  // Get addresses that match host and port
  s = getaddrinfo( argv[1], argv[2], &hints, &result );

  if (s != 0) {
     fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( s ) );
     exit( EXIT_FAILURE );
  }

  // Loop through list of addresses and try to connect
  for ( rp = result; rp != NULL; rp = rp->ai_next ) {
    // Create a socket
    sfd = socket( rp->ai_family, rp->ai_socktype, rp->ai_protocol );

    // Failed to create socket
    if ( sfd == -1 )
      continue;

    // Try to bind
    if ( bind( sfd, rp->ai_addr, rp->ai_addrlen ) != -1)
      break;

    // Close this socket if we could not connect
    close( sfd );
  }

  // Check if we're connected
  if ( rp == NULL ) {
     fprintf( stderr, "Could not connect\n" );
     exit( EXIT_FAILURE );
  }
  
  // Free memory
  freeaddrinfo( result );

  // Now the reading of the structure can begin
  while ( 1 ) {
    nread = read( sfd, buf, BUF_SIZE );
    if ( nread == -1 ) {
      perror( "Error while reading" );
      exit( EXIT_FAILURE );
    }
    
    // TODO: Check header
    fillGCData( buf, &GCData );
    
    // Print statements for debugging purposes
    printf( "State            = %d\n", GCData.state );
    printf( "firstHalf        = %d\n", GCData.firstHalf );
    printf( "secondaryState   = %d\n", GCData.secondaryState );
    printf( "dropInTeam       = %d\n", GCData.dropInTeam );
    printf( "dropInTime       = %d\n", GCData.dropInTime );
    printf( "secsRemaining    = %d\n", GCData.secsRemaining );
    printf( "%d - %d\n", GCData.teams[0].score, GCData.teams[1].score );
  }

  close( sfd );
  exit( EXIT_SUCCESS );
}

/*
 * Fills the given structure with the values from the buffer. Assumes that
 * values are little-endian.
 *
 * Note: Explicit use of non postfix increments, as they can have weird side-
 * effects when used in combined expressions.
 */
void fillGCData( char buf[], struct RoboCupGameControlData * GCData ) {
  // Warning: order of bytes!
  // Note that all characters are implicitly converted to ints
  int i = 4;
  
  // Read global data
  GCData->version        = ( buf[i]   << 0  ) | 
                           ( buf[i+1] << 8  ) | 
                           ( buf[i+2] << 16 ) | 
                           ( buf[i+3] << 32 );
  i += 4;
  GCData->playersPerTeam = buf[i++];
  GCData->state          = buf[i++];
  GCData->firstHalf      = buf[i++];
  GCData->kickOffTeam    = buf[i++];
  GCData->secondaryState = buf[i++];
  GCData->dropInTeam     = buf[i++];
  GCData->dropInTime     = ( buf[i]   << 0  ) | 
                           ( buf[i+1] << 8  );
  i += 2;
  GCData->secsRemaining  = ( buf[i]   << 0  ) | 
                           ( buf[i+1] << 8  ) | 
                           ( buf[i+2] << 16 ) | 
                           ( buf[i+3] << 32 );
  i += 4;

  // Read data per team
  for ( int team = 0; team < 2; team++ ) {
    GCData->teams[team].teamNumber = buf[i++];
    GCData->teams[team].teamColour = buf[i++];
    GCData->teams[team].goalColour = buf[i++];
    GCData->teams[team].score      = buf[i++];
    
    // Read data per player/robot
    for ( int bot = 0; bot < MAX_NUM_PLAYERS; bot++ ) {
      GCData->teams[team].players[bot].penalty             = ( buf[i]   << 0 ) |
                                                             ( buf[i+1] << 8 );
      i += 2;
      GCData->teams[team].players[bot].secsTillUnpenalised = ( buf[i]   << 0 ) |
                                                             ( buf[i+1] << 8 );
      i += 2;
    }
  }
}
