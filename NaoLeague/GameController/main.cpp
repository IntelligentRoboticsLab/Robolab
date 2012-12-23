/*
 * Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the COPYING file.
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

void slice( char data[], char sliced[], int begin, int end );

int main( int argc, char *argv[] ) {
  struct addrinfo hints;
  struct addrinfo *result, *rp;

  struct RoboCupGameControlData GCData;

  int sfd, s, j;
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
    if ( bind(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
      break;

    // Close this socket if we could not connect
    close( sfd );
  }

  // Check if we're connected
  if ( rp == NULL ) {
     fprintf( stderr, "Could not connect\n");
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

    // Warning: order of bytes!
    GCData.version        = ( buf[4] << 0  ) | 
                            ( buf[5] << 8  ) | 
                            ( buf[6] << 16 ) | 
                            ( buf[7] << 32 );
    GCData.playersPerTeam = buf[8];
    GCData.state          = buf[9];
    GCData.firstHalf      = buf[10];
    GCData.kickOffTeam    = buf[11];
    GCData.secondaryState = buf[12];
    GCData.dropInTeam     = buf[13];
    GCData.dropInTime     = ( buf[14] << 0 ) | 
                            ( buf[15] << 0 );
    GCData.secsRemaining  = ( buf[16] << 0 ) | 
                            ( buf[17] << 8 ) | 
                            ( buf[18] << 16  ) | 
                            ( buf[19] << 32  );

    // Print statements for debugging purposes
    printf( "Version          = %d\n", GCData.version );
    printf( "Players per team = %d\n", GCData.playersPerTeam );
    printf( "State            = %d\n", GCData.state );
    printf( "firstHalf        = %d\n", GCData.firstHalf );
    printf( "KickOffTeam      = %d\n", GCData.kickOffTeam );
    printf( "secondaryState   = %d\n", GCData.secondaryState );
    printf( "dropInTeam       = %d\n", GCData.dropInTeam );
    printf( "dropInTime       = %d\n", GCData.dropInTime );
    printf( "secsRemaining    = %d\n", GCData.secsRemaining );

    // TODO: Read team info

    printf( "Received %ld bytes: %s\n", (long) nread, buf );  
  }

  close( sfd );
  exit(EXIT_SUCCESS);
}

