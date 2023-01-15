/* ******************************************** */
/* File Name: dict-test.c                       */
/* Author: (C) 2008-2009 - Juan José Ponteprino */
/* Description: Dictionary API Headers          */
/* License: This code is licenced as LGPL       */
/* ******************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dict.h>

int main( int argc, char * argv[] )
{
    DICT_T * main_dict = NULL;
    char * val1 = "hello world";
    char * val2 = "goodbye world";
    char * val3 = "works!";

    DICT_Init();

    main_dict = DICT_Create( "main" );
    if ( !main_dict )
    {
        fprintf( stderr, "Can't create dictionary\n" );
        exit( -1 );
    }

    printf( "\n" );
    printf( "Add a new entry... foo1... ret: %d\n", DICT_AddEntry( main_dict, "foo1", ( void * ) val1, strlen( val1 ) + 1, DICT_ENTRY_FLAG_NORMAL ));

    printf( "\n" );
    printf( "Get an exists entry... foo1... ret: [%s]\n", DICT_GetEntry( main_dict, "foo1", NULL ));
    printf( "Get non exist entry... foo2... ret: [%s]\n", DICT_GetEntry( main_dict, "foo2", NULL ));

    printf( "\n" );
    printf( "Find an entry... foo1... ret: %p\n", DICT_FindEntry( main_dict, "foo1" ) );
    printf( "Find non exists entry... foo2... ret: %p\n", DICT_FindEntry( main_dict, "foo2" ) );

    printf( "\n" );
    printf( "Update an exists entry... foo1... ret: %d\n", DICT_AddEntry( main_dict, "foo1", ( void * ) val2, strlen( val2 ) + 1, DICT_ENTRY_FLAG_NORMAL ));

    printf( "\n" );
    printf( "Get an exists entry (with a large value)... foo1... ret: [%s]\n", DICT_GetEntry( main_dict, "foo1", NULL ));
    printf( "Find an entry... foo1... ret: %p\n", DICT_FindEntry( main_dict, "foo1" ) );

    printf( "\n" );
    printf( "Update an exists entry (with a short value)... foo1... ret: %d\n", DICT_AddEntry( main_dict, "foo1", ( void * ) val3, strlen( val3 ) + 1, DICT_ENTRY_FLAG_NORMAL ));

    printf( "\n" );
    printf( "Get an exists entry... foo1... ret: [%s]\n", DICT_GetEntry( main_dict, "foo1", NULL ));
    printf( "Find an entry... foo1... ret: %p\n", DICT_FindEntry( main_dict, "foo1" ) );

    printf( "\n" );
    printf( "Delete an exists entry...\n" );
    DICT_DelEntry( main_dict, "foo1" );

    printf( "\n" );
    printf( "Delete a non exists entry...\n" );
    DICT_DelEntry( main_dict, "foo1" );

    printf( "\n" );
    printf( "Get a non exists entry... foo1... ret: %p\n", DICT_GetEntry( main_dict, "foo1", NULL ));
    printf( "Find a non exists entry... foo1... ret: [%s]\n", DICT_FindEntry( main_dict, "foo1" ));

    printf( "\n" );
    printf( "Add a new entry... foo1... ret: %d\n", DICT_AddEntry( main_dict, "foo1", ( void * ) val1, strlen( val1 ) + 1, DICT_ENTRY_FLAG_NORMAL ));
    printf( "Add a new entry... foo2... ret: %d\n", DICT_AddEntry( main_dict, "foo2", ( void * ) val2, strlen( val2 ) + 1, DICT_ENTRY_FLAG_NORMAL ));
    printf( "Add a new entry... foo3... ret: %d\n", DICT_AddEntry( main_dict, "foo3", ( void * ) val3, strlen( val3 ) + 1, DICT_ENTRY_FLAG_NORMAL ));

    printf( "\n" );
    printf( "Display all Entries\n" );
    DICT_ShowAllEntries( main_dict, stdout );

    printf( "\n" );
    printf( "Delete all Entries\n" );
    DICT_DelAllEntries( main_dict );

    printf( "\n" );
    printf( "Display all Entries\n" );
    DICT_ShowAllEntries( main_dict, stdout );

    DICT_Destroy( main_dict );

    DICT_Exit();

    return 0;
}
