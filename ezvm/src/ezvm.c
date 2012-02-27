/* ------------------------------------------------------------------- */
/*  ACCORDS PLATFORM                                                   */
/*  (C) 2011 by Iain James Marshall (Prologue) <ijm667@hotmail.com>    */
/* --------------------------------------------------------------------*/
/*  This is free software; you can redistribute it and/or modify it    */
/*  under the terms of the GNU Lesser General Public License as        */
/*  published by the Free Software Foundation; either version 2.1 of   */
/*  the License, or (at your option) any later version.                */
/*                                                                     */
/*  This software is distributed in the hope that it will be useful,   */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of     */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   */
/*  Lesser General Public License for more details.                    */
/*                                                                     */
/*  You should have received a copy of the GNU Lesser General Public   */
/*  License along with this software; if not, write to the Free        */
/*  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA */
/*  02110-1301 USA, or see the FSF site: http://www.fsf.org.           */
/* --------------------------------------------------------------------*/
#ifndef	_ezvm_c
#define	_ezvm_c

#include "standard.h"
#include "ezvm.h"
#include "rest.h"
#include "occi.h"
#include "document.h"
#include "cordspublic.h"
#include "occipublisher.h"
#include "occibuilder.h"

struct	accords_configuration Ezvm = {
	0,0, 
	0,0,0,
	(char *) 0,
	(char *) 0,
	_CORDS_DEFAULT_PUBLISHER,
	_CORDS_DEFAULT_OPERATOR,
	_CORDS_DEFAULT_USER,
	_CORDS_DEFAULT_PASSWORD,
	"http",  80,
	"xmpp",  8000,
	"domain",
	"ezvm.xml",
	(struct occi_category *) 0,
	(struct occi_category *) 0
	};

public	int	check_debug()		{	return(Ezvm.debug);		}
public	int	check_verbose()		{	return(Ezvm.verbose);		}
public	char *	default_publisher()	{	return(Ezvm.publisher);	}
public	char *	default_operator()	{	return(Ezvm.operator);	}
public	char *	default_tls()		{	return(Ezvm.tls);		}

public	int	failure( int e, char * m1, char * m2 )
{
	if ( e )
	{
		printf("\n*** failure %u",e);
		if ( m1 )
			printf(" : %s",m1);
		if ( m2 )
			printf(" : %s",m2);
		printf(" **`\n");
	}
	return( e );
}

/*	---------------------------------------------	*/  
/*			e z v m _ l o a d 		*/
/*	---------------------------------------------	*/
/*	this function loads ezvm    configuration	*/
/*	from the xml configuration file.		*/
/*	---------------------------------------------	*/  
private	void	ezvm_load()
{
	load_accords_configuration( &Ezvm, "ezvm" );
	return;
}

private	int	banner()
{
	printf("\n   CompatibleOne Ezvm : Version 1.0a.0.01");
	printf("\n   Beta Version : 07/01/2012");
	printf("\n   Copyright (c) 2012 Iain James Marshall, Prologue");
	printf("\n");
	accords_configuration_options();
	printf("\n\n");
	return(0);

}

/*	------------------------------------------------------------------	*/
/*			e z v m _ i n i t i a l i s e			*/
/*	------------------------------------------------------------------	*/
private	struct rest_server * ezvm_initialise(  void * v,struct rest_server * sptr )
{
	struct	rest_extension * xptr;
	if (!( xptr = rest_add_extension( sptr ) ))
		return((struct rest_server *) 0);
	else
	{
		xptr->net = (struct connection *) 0;
		return( sptr );
	}
}

/*	------------------------------------------------------------------	*/
/*			e z v m _ a u t h o r i s e 			*/
/*	------------------------------------------------------------------	*/
private	int	ezvm_authorise(  void * v,struct rest_client * cptr, char * username, char * password )
{
	if ( strcmp( username, Ezvm.user ) )
		return(0);
	else if ( strcmp( password, Ezvm.password ) )
		return(0);
	else if (!( cptr->user = allocate_string( username ) ))
		return(0);
	else if (!( cptr->pass = allocate_string( password ) ))
		return(0);
	else	return(1);
}

/*	------------------------------------------------------------------	*/
/*			e z v m _ e x t e n s i o n 			*/
/*	------------------------------------------------------------------	*/
private	struct rest_extension * ezvm_extension( void * v,struct rest_server * sptr, struct rest_extension * xptr)
{
	return( xptr );
}

/*	------------------------------------------------------------------	*/
/*	inclusion of the actions and methods required for contract handling	*/
/*	------------------------------------------------------------------	*/

/*	------------------------------------------------------------------	*/
/*			e z v m _ o p e r a t i o n				*/
/*	------------------------------------------------------------------	*/
private	int	ezvm_operation( char * nptr )
{

	struct	occi_category * first=(struct occi_category *) 0;
	struct	occi_category * last=(struct occi_category *) 0;
	struct	occi_category * optr=(struct occi_category *) 0;

	set_autosave_cords_xlink_name("links_ezvm.xml");

	if (!( optr = occi_cords_vm_builder( Ezvm.domain,"vm" ) ))
		return( 27 );
	else if (!( optr->previous = last ))
		first = optr;
	else	optr->previous->next = optr;
	last = optr;

	if (!( optr = occi_cords_image_builder( Ezvm.domain,"image" ) ))
		return( 27 );
	else if (!( optr->previous = last ))
		first = optr;
	else	optr->previous->next = optr;
	last = optr;

	if (!( optr = occi_cords_system_builder( Ezvm.domain,"system" ) ))
		return( 27 );
	else if (!( optr->previous = last ))
		first = optr;
	else	optr->previous->next = optr;
	last = optr;
	if (!( optr = occi_cords_package_builder( Ezvm.domain,"package" ) ))
		return( 27 );
	else if (!( optr->previous = last ))
		first = optr;
	else	optr->previous->next = optr;
	last = optr;

	rest_initialise_log( Ezvm.monitor );

	if (!( Ezvm.identity ))
		return( occi_server(  nptr, Ezvm.restport, Ezvm.tls, Ezvm.threads, first, (char *) 0 ) );
	else
	{
		initialise_occi_publisher( Ezvm.publisher, (char*) 0, (char *) 0, (char *) 0);
		return( publishing_occi_server(
			Ezvm.user, Ezvm.password,
			Ezvm.identity,  nptr, 
			Ezvm.restport, Ezvm.tls, 
			Ezvm.threads, first ) );
	}
}


/*	------------------------------------------------------------------	*/
/*				e z v m 					*/
/*	------------------------------------------------------------------	*/
private	int	ezvm(int argc, char * argv[] )
{
	int	status=0;
	int	argi=0;
	char *	aptr;
	ezvm_load();
	while ( argi < argc )
	{
		if (!( aptr = argv[++argi] ))
			break;
		else if ( *aptr == '-' )
		{
			aptr++;
			switch( *(aptr++) )
			{
			case	'v'	:
				Ezvm.verbose=1;
				continue;
			case	'd'	:
				Ezvm.debug = 0xFFFF;
				continue;
			case	'-'	:
				if (!( argi = accords_configuration_option( aptr, argi, argv )))
					break;
				else	continue;
			}
			status = 30;
			break;
		}
		else if (!( status = ezvm_operation(aptr) ))
			continue;
		else	break;
	}
	return(status);
}

/*	------------------------------------------------------------------	*/
/*					m a i n 				*/
/*	------------------------------------------------------------------	*/
public	int	main(int argc, char * argv[] )
{
	if ( argc == 1 )
		return( banner() );
	else	return( ezvm( argc, argv ) );
}


	/* --------- */
#endif	/* _ezvm_c */
	/* --------- */
