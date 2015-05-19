/*
 *  This file is a part of the PARUS project.
 *  Copyright (C) 2006  Alexey N. Salnikov (salnikov@cmc.msu.ru)
 *  
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

#include <netcdf.h>

#include "network_speed.h"
#include "str_operation.h"

#include "data_write_operations.h"

 const char *parus_default_file_name_prefix = "network";


#define READ_STR_LENGTH 300

/***********************************************************************/
Network_speed::Network_speed()
{
    state = info_state_nothing;

    num_processors=0;
    num_messages=0;

    /* Test parameters */
    test_type_name[0] = 0;
	data_type[0] = 0;
    host_names = 0;
    host_ranks = 0;

    test_parameters.num_procs            =  0; /* Special for program break on any error */
    test_parameters.test_type            =  ALL_TO_ALL_TEST_TYPE;
    test_parameters.begin_message_length =  MESSAGE_BEGIN_LENGTH;
    test_parameters.end_message_length   =  MESSAGE_END_LENGTH;
    test_parameters.step_length          =  MESSAGE_STEP;
    test_parameters.num_repeats          =  NUM_REPEATS;
    test_parameters.noise_message_length =  NOISE_MESSAGE_LENGTH;
    test_parameters.num_noise_messages   =  NOISE_MESSAGE_NUM;
    test_parameters.num_noise_procs      =  NUM_NOISE_PROCS;
    test_parameters.file_name_prefix     =  parus_default_file_name_prefix;

    mass_size_of_msg[0]=10;
    mass_size_of_msg[1]=100;
    mass_size_of_msg[2]=1000;
    mass_size_of_msg[3]=3000;
    mass_size_of_msg[4]=6000;
    mass_size_of_msg[5]=10000;
    
    //messages_length=NULL;
    //links=NULL;
    return;
}
/***********************************************************************/
Network_speed::~Network_speed()
{
	if ( host_names != NULL )
    {
        for ( int i = 0; i < num_processors; i++ )
		{
			if(host_names[i]!=NULL)
			{
				free( host_names[i] );
				host_names[i]=0;
			}
		}
        free( host_names );
		host_names=NULL;
    }

    if ( host_ranks != NULL )
	free( host_ranks );

    num_processors=0;
    num_messages=0;

    return;
}
/****************************************************************************/
int Network_speed::fread(char *file_name)
{
    /*FILE *f;
    int flag;
    char str[READ_STR_LENGTH];
    int i;
    state = info_state_no_file;
    f=fopen(file_name,"r");
    if(f==NULL)
    {
        printf("Network_speed::fread(char *) can not open file '%s'\n",file_name);
        return -1;
    }
    flag=get_word(f,str,READ_STR_LENGTH);
    if(flag)
        return -1;
    if(strcmp(str,"processors"))
    {
        printf("Bad format file %s\n",file_name);
        printf(" 'processor' record are not present '%s'\n",str);
        return -1;
    }

    flag=get_word(f,str,READ_STR_LENGTH);
    if(flag)
        return -1;
    num_processors=atoi(str);
    // Update state
    state = info_state_processors;

    // /* After revision 61 file header contains only processor number
     * 
     * flag=get_word(f,str);
     * if(flag==-1) return -1;
     * if(strcmp(str,"num"))
     * {
     *  printf("Bad format file %s\n",file_name);
     *  printf(" record 'num' not precent '%s'\n",str);
     *  return -1;
     * }
     * 
     * flag=get_word(f,str);
     * if(flag==-1) return -1;
     * if(strcmp(str,"messages"))
     * {
     *  printf("Bad format file %s\n",file_name);
     *  return -1;
     * }
     *
     * flag=get_word(f,str);
     * if(flag==-1) return -1;
     * num_messages=atoi(str);
     *
     // */

    // /* Test parameters */
     /*
    #define GETWORD flag = get_word( f, str , READ_STR_LENGTH ); if ( flag ) return -1;
    #define FORMATCHECK(x) if ( strcmp( str, x ) ) { printf( "Bad format file %s\n", file_name ); return -1; }


    GETWORD FORMATCHECK( "test" )
    GETWORD FORMATCHECK( "type" )
    flag = read_string( f, test_type, READ_STR_LENGTH );
    if ( flag )
        return -1;

	GETWORD FORMATCHECK( "data" )
	GETWORD FORMATCHECK( "type" )
	flag = read_string( f, test_type, READ_STR_LENGTH );
	if ( flag )
		return -1;

    GETWORD FORMATCHECK( "begin" )
    GETWORD FORMATCHECK( "message" )
    GETWORD FORMATCHECK( "length" )
    GETWORD begin_message_length = atoi( str );

    GETWORD FORMATCHECK( "end" )
    GETWORD FORMATCHECK( "message" )
    GETWORD FORMATCHECK( "length" )
    GETWORD end_message_length = atoi( str );

    GETWORD FORMATCHECK( "step" )
    GETWORD FORMATCHECK( "length" )
    GETWORD step_length = atoi( str );

    GETWORD FORMATCHECK( "noise" )
    GETWORD FORMATCHECK( "message" )
    GETWORD FORMATCHECK( "length" )
    GETWORD noise_message_length = atoi( str );

    GETWORD FORMATCHECK( "number" )
    GETWORD FORMATCHECK( "of" )
    GETWORD FORMATCHECK( "noise" )
    GETWORD FORMATCHECK( "messages" )
    GETWORD noise_message_num = atoi( str );

    GETWORD FORMATCHECK( "number" )
    GETWORD FORMATCHECK( "of" )
    GETWORD FORMATCHECK( "noise" )
    GETWORD FORMATCHECK( "processes" )
    GETWORD noise_processors = atoi( str );

    GETWORD FORMATCHECK( "number" )
    GETWORD FORMATCHECK( "of" )
    GETWORD FORMATCHECK( "repeates" )
    GETWORD num_repeats = atoi( str );
    
    ///*
     * Commented by Alexey Salnikov.
     *
     * I think commented code fragment is abuse for file format.
    // */
    
    /*
    GETWORD FORMATCHECK( "result" )
    GETWORD FORMATCHECK( "file" )
    flag = read_string( f, str );
    if ( flag == -1 ) return -1;
    */
    /*
    GETWORD FORMATCHECK( "hosts:" )
    
    // Update state
    state = info_state_test_parameters;

    /* Reading host names, each processor - one host name */
    /*host_names = (char**)malloc(sizeof(char*)*num_processors);
    host_ranks = (int*)malloc(sizeof(int)*num_processors);
    for ( int i = 0; i < num_processors; i++ )
        host_names[i] = (char*)malloc( 256 * sizeof(char));
    for ( int i = 0; i < num_processors; i++ )
    {
        flag = get_word( f, host_names[i] , READ_STR_LENGTH );
        if ( flag ) return -1;
	
	/*
	 * 
	 * Why we need to describe host rank in file?
	 *
	 * The order of hosts in a file sets natural order of hosts in
	 * a MPI-programm.
	 * 
	 */
	/*
	GETWORD FORMATCHECK( "rank" )
	GETWORD host_ranks[i] = atoi( str );
	*/
    /*}

    #undef GETWORD
    #undef FORMATCHECK

    int tmp_num_messages = 0;
    for( int i = begin_message_length; i < end_message_length; tmp_num_messages++ )
        i += step_length;
    messages_length=(int *)malloc(sizeof(int)*tmp_num_messages);
    if(messages_length==NULL)
    {
        printf("Network_speed::fread(char *) Out of the memory\n");
        return -1;
    }

    links=new  Matrix [tmp_num_messages];
    if(links==NULL)
    {
        printf("Network_speed::fread(char *) Out of the memory\n");
        return -1;
    }

    num_messages = 0;
    for(i=0;i<tmp_num_messages;i++)
    {
        flag=get_word(f,str,READ_STR_LENGTH);
        if(flag)
            return -2;
        if(strcmp(str,"Message"))
        {
            printf("Incomplete file %s\n",file_name);
            return -2;
        }

        flag=get_word(f,str,READ_STR_LENGTH);
        if(flag)
            return -2;
        if(strcmp(str,"length"))
        {
            printf("Incomplete file %s\n",file_name);
            return -2;
        }

        flag=get_word(f,str,READ_STR_LENGTH);
        if(flag)
            return -2;
        messages_length[i]=atoi(str);

        flag=links[i].fread(f,num_processors,num_processors);
        if(flag)
        {
            printf("Can not read matrix for the %d message length\n",messages_length[i]);
            return -2;
        }

        num_messages++; // So, we got matrix for another one message length
    }

    // Update state
    if ( num_messages == tmp_num_messages )
        state = info_state_partial_matrices;
    else
        state = info_state_all_done;

    // Close file!
    fclose(f);
    */
    return 0;
}
/****************************************************************************/
/****************************************************************************/
int Network_speed::make_file(char *file_name)
{
    MPI_Status status;
    int flag;
    Test_time_result_type *times=NULL;
    int tmp_mes_size;
    int step_num=0;

    MPI_Datatype MPI_My_time_struct;
    MPI_Datatype struct_types[4]= {MPI_DOUBLE,MPI_DOUBLE,MPI_DOUBLE,MPI_DOUBLE};
    int blocklength[4]= {1,1,1,1/*,1*/};
    MPI_Aint displace[4],base;

    MPI_Comm_size(MPI_COMM_WORLD,&comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD,&comm_rank);
    if(comm_rank == 0)
    {
        if ( comm_size == 1 )
        {
            printf( "\n\nYou tries to run this programm for one MPI thread!\n\n" );
        }

        host_names = (char**)malloc(sizeof(char*)*comm_size);
        if(host_names==NULL)
        {
            printf("Can't allocate memory %d bytes for host_names\n",(int)(sizeof(char*)*comm_size));
            MPI_Abort(MPI_COMM_WORLD,-1);
            return -1;
        }

        for (int i = 0; i < comm_size; i++ )
        {
            host_names[i] = (char*)malloc(256*sizeof(char));
            if(host_names[i]==NULL)
            {
                printf("Can't allocate memory for name proc %d\n",i);
                MPI_Abort(MPI_COMM_WORLD,-1);
            }
        }
    } /* End if(rank==0) */

    /*
     * Going to get and write all processors' hostnames
     */
    gethostname( host_name, 255 );

    if ( comm_rank == 0 )
    {
        for (int i = 1; i < comm_size; i++ )
            MPI_Recv( host_names[i], 256, MPI_CHAR, i, 200, MPI_COMM_WORLD, &status );
        strcpy(host_names[0],host_name);
    }
    else
    {
        MPI_Send( host_name, 256, MPI_CHAR, 0, 200, MPI_COMM_WORLD );
    }

    /*
     * Initializing num_procs parameter
     */
    test_parameters.num_procs=comm_size;

    if( comm_rank == 0)
    {
        /*
         *
         * Matrices initialization
         *
         */
        mtr_av = (Easy_matrix*)malloc(sizeof(Easy_matrix));
        flag = easy_mtr_create(mtr_av,comm_size,comm_size);
        if( flag==-1 )
        {
            printf("Can not to create average matrix to story the test results\n");
            MPI_Abort(MPI_COMM_WORLD,-1);
            return -1;
        }
        mtr_me = (Easy_matrix*)malloc(sizeof(Easy_matrix));
        flag = easy_mtr_create(mtr_me,comm_size,comm_size);
        if( flag==-1 )
        {
            printf("Can not to create median matrix to story the test results\n");
            MPI_Abort(MPI_COMM_WORLD,-1);
            return -1;
        }
        mtr_di = (Easy_matrix*)malloc(sizeof(Easy_matrix));
        flag = easy_mtr_create(mtr_di,comm_size,comm_size);
        if( flag==-1 )
        {
            printf("Can not to create deviation matrix to story the test results\n");
            MPI_Abort(MPI_COMM_WORLD,-1);
            return -1;
        }
        mtr_mi = (Easy_matrix*)malloc(sizeof(Easy_matrix));
        flag = easy_mtr_create(mtr_mi,comm_size,comm_size);
        if( flag==-1 )
        {
            printf("Can not to create min values matrix to story  the test results\n");
            MPI_Abort(MPI_COMM_WORLD,-1);
            return -1;
        }

        /*if(create_netcdf_header(AVERAGE_NETWORK_TEST_DATATYPE,&test_parameters,&netcdf_file_av,&netcdf_var_av))
        {
            printf("Can not to create file with name \"%s_average.nc\"\n",test_parameters.file_name_prefix);
            MPI_Abort(MPI_COMM_WORLD,-1);
            return -1;
        }

        if(create_netcdf_header(MEDIAN_NETWORK_TEST_DATATYPE,&test_parameters,&netcdf_file_me,&netcdf_var_me))
        {
            printf("Can not to create file with name \"%s_median.nc\"\n",test_parameters.file_name_prefix);
            MPI_Abort(MPI_COMM_WORLD,-1);
            return -1;
        }

        if(create_netcdf_header(DEVIATION_NETWORK_TEST_DATATYPE,&test_parameters,&netcdf_file_di,&netcdf_var_di))
        {
            printf("Can not to create file with name \"%s_deviation.nc\"\n",test_parameters.file_name_prefix);
            MPI_Abort(MPI_COMM_WORLD,-1);
            return -1;
        }

        if(create_netcdf_header(MIN_NETWORK_TEST_DATATYPE,&test_parameters,&netcdf_file_mi,&netcdf_var_mi))
        {
            printf("Can not to create file with name \"%s_min.nc\"\n",test_parameters.file_name_prefix);
            MPI_Abort(MPI_COMM_WORLD,-1);
            return -1;
        }*/

        if(create_test_hosts_file(&test_parameters,host_names))     
        {
            printf("Can not to create file with name \"%s_hosts.txt\"\n",test_parameters.file_name_prefix);
                MPI_Abort(MPI_COMM_WORLD,-1);
                return -1;
        }

        /*
         *
         * Printing initial message for user
         *
         */
         //???
        //!get_test_type_name(test_parameters.test_type,test_type_name);
    } /* End preparation (only in MPI process with rank 0) */

    /*
     * Broadcasting command line parametrs
     *
     * The structure network_test_parameters_struct contains 9
     * parametes those are placed at begin of structure.
     * So, we capable to think that it is an array on integers.
     *
     * Little hack from Alexey Salnikov.
     */
    MPI_Bcast(&test_parameters,9,MPI_INT,0,MPI_COMM_WORLD);


    /*
     * Creating struct time type for MPI operations
     */
    {
        Test_time_result_type tmp_time;
        MPI_Address( &(tmp_time.average), &base);
        MPI_Address( &(tmp_time.median), &displace[1]);
        MPI_Address( &(tmp_time.deviation), &displace[2]);
        MPI_Address( &(tmp_time.min), &displace[3]);
    }
    displace[0]=0;
    displace[1]-=base;
    displace[2]-=base;
    displace[3]-=base;
    MPI_Type_struct(4,blocklength,displace,struct_types,&MPI_My_time_struct);
    MPI_Type_commit(&MPI_My_time_struct);


    times=(Test_time_result_type* )malloc(comm_size*sizeof(Test_time_result_type));
    if(times==NULL)
    {
        printf("Memory allocation error\n");
        MPI_Abort(MPI_COMM_WORLD,-1);
        return -1;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    

    /*
     * Circle by length of messages
     */
    for (int i=0; i<6;i++)//идём по mass_size_of_msg
        /*(
         tmp_mes_size=test_parameters.begin_message_length;
         tmp_mes_size<test_parameters.end_message_length;
         step_num++,tmp_mes_size+=test_parameters.step_length
        )*/
    {
        if(test_parameters.test_type==ALL_TO_ALL_TEST_TYPE)
        {
            all_to_all(times,mass_size_of_msg[i],2); //repeat test for accuracy
        }

        MPI_Barrier(MPI_COMM_WORLD);

        if(comm_rank==0)
        {
            for(int j=0; j<comm_size; j++)
            {
                MATRIX_FILL_ELEMENT((mtr_av*),0,j,times[j].average);
                MATRIX_FILL_ELEMENT((mtr_me*),0,j,times[j].median);
                MATRIX_FILL_ELEMENT((mtr_di*),0,j,times[j].deviation);
                MATRIX_FILL_ELEMENT((mtr_mi*),0,j,times[j].min);
            }
            for(int i=1; i<comm_size; i++)
            {

                MPI_Recv(times,comm_size,MPI_My_time_struct,i,100,MPI_COMM_WORLD,&status);
                for(int j=0; j<comm_size; j++)
                {
                    MATRIX_FILL_ELEMENT((mtr_av*),i,j,times[j].average);
                    MATRIX_FILL_ELEMENT((mtr_me*),i,j,times[j].median);
                    MATRIX_FILL_ELEMENT((mtr_di*),i,j,times[j].deviation);
                    MATRIX_FILL_ELEMENT((mtr_mi*),i,j,times[j].min);

                }
            }


            /*if(netcdf_write_matrix(netcdf_file_av,netcdf_var_av,i,mtr_av->sizex,mtr_av->sizey,mtr_av->body))
            {
                printf("Can't write average matrix to file.\n");
                MPI_Abort(MPI_COMM_WORLD,-1);
                return 1;
            }

            if(netcdf_write_matrix(netcdf_file_me,netcdf_var_me,i,mtr_me->sizex,mtr_me->sizey,mtr_me->body))
            {
                printf("Can't write median matrix to file.\n");
                MPI_Abort(MPI_COMM_WORLD,-1);
                return 1;
            }

            if(netcdf_write_matrix(netcdf_file_di,netcdf_var_di,i,mtr_di->sizex,mtr_di->sizey,mtr_di->body))
            {
                printf("Can't write deviation matrix to file.\n");
                MPI_Abort(MPI_COMM_WORLD,-1);
                return 1;
            }

            if(netcdf_write_matrix(netcdf_file_mi,netcdf_var_mi,i,mtr_mi->sizex,mtr_mi->sizey,mtr_mi->body))
            {
                printf("Can't write  matrix with minimal values to file.\n");
                MPI_Abort(MPI_COMM_WORLD,-1);
                return 1;
            }*/

            matrixs.insert ( std::pair<int,Easy_matrix*>(mass_size_of_msg[i],mtr_me) );
            mtr_me = (Easy_matrix*)malloc(sizeof(Easy_matrix));

            flag = easy_mtr_create(mtr_me,comm_size,comm_size);
            if( flag==-1 )
            {
                printf("Can not to create median matrix to story the test results\n");
                MPI_Abort(MPI_COMM_WORLD,-1);
                return -1;
            }

            printf("message length %d finished\r",mass_size_of_msg[i]);
            fflush(stdout);

        } /* end comm rank 0 */
        else
        {
            MPI_Send(times,comm_size,MPI_My_time_struct,0,100,MPI_COMM_WORLD);
        }

      
        /* end for cycle .
         * Now we  go to the next length of message that is used in
         * the test perfomed on multiprocessor.
         */
    }
    
    /* TODO
     * Now free times array.
     * It should be changed in future for memory be allocated only once.
     *
     * Times array should be moved from return value to the input argument
     * for any network_test.
     */
    if(comm_rank==0)
    {
        free(mtr_av.body);
        //free(mtr_me.body);
        free(mtr_di.body);
        free(mtr_mi.body);

        printf("\nTest is done\n");
    }

    free(times);
}

/*int Network_speed::read_file(char *file_name)
{
    //create_netcdf_header
}*/

//***************************************************
int Network_speed::close_and_free()
{
    //if(comm_rank==0)
    netcdf_close_file(netcdf_file_av);
    netcdf_close_file(netcdf_file_me);
    netcdf_close_file(netcdf_file_di);
    netcdf_close_file(netcdf_file_mi);

    for(int i=0; i<comm_size; i++)
    {
        free(host_names[i]);
    }
    free(host_names);
    for(int i=0; i<6; i++)
    {
        free(matrixs[mass_size_of_msg[i]]);
    }

    return 0;
}

double Network_speed::translate_time(int from,int to,int length)//NEED NEW FUNCTION
{
	int i, j=0;
	
	/*
	 * There is not line interpolation scheme.
	 *
	 * We discussed does line interpolation nesessary 
	 * here and decide it will be other scheme.
	 * 
	 */
	
    double otv;

    for (i=0; i<6;i++)
    {
        if(length <= mass_size_of_msg[i])
        {
            return MATRIX_GET_ELEMENT(matrixs[mass_size_of_msg[i]],from,to)
        }
    }
    return MATRIX_GET_ELEMENT(matrixs[mass_size_of_msg[6]],from,to)
	
    //uncomment it if you want to read info from nc-files
    /*if(i>=test_parameters.end_message_length)
	{
        size_t dims[] = {j-1, from, to};
		nc_get_var1_double(netcdf_file_me, netcdf_var_me, dims, &otv);
        return otv;
	}
	size_t dims[] = {j-1, from, to};
    nc_get_var1_double(netcdf_file_me, netcdf_var_me, dims, &otv);
    return otv;*/
	//return 0.0;//links[i].element(from,to);
}
/****************************************************************************/

