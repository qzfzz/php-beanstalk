#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main()
{	
	char buf[] = "INSERTED,112314513";
	char* token = strtok( buf, "," );

	int bRet = 0;
	int iRet = 0;
	while( token != NULL )
	{
		printf( "%s\n", token );
		if( !strncmp( "INSERTED", token, sizeof("INSERTED") - 1))
		{
			bRet = 1;
			token = strtok( NULL, "," );
			continue;
		}

		if( iRet = atoi( token ))
		{
			return iRet;
			break;
		}

		token = strtok( NULL, "," );
	}
	return 0;
}
