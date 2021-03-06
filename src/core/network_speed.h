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

#ifndef __NETWORK_SPEED_H_
#define __NETWORK_SPEED_H_

#include "linealge.h"
#include "my_time.h"
#include <string.h>
#include <map>

#include "string_id_converters.h"
#include "tests_common.h"
#include "easy_matrices.h"
#include "my_malloc.h"
#include "types.h"
#include "../../parus_config.h"


#define MESSAGE_BEGIN_LENGTH 100
#define MESSAGE_END_LENGTH 3100
#define NUM_REPEATS 1
#define MESSAGE_STEP 500
#define NOISE_MESSAGE_LENGTH 0
#define NOISE_MESSAGE_NUM 1
#define NUM_NOISE_PROCS 0

#define UNKNOWN_TEST_TYPE 0
#define ONE_TO_ONE_TEST_TYPE 1
#define ALL_TO_ALL_TEST_TYPE 2
#define ASYNC_ONE_TO_ONE_TEST_TYPE 3
#define SEND_RECV_AND_RECV_SEND_TEST_TYPE 4
#define NOISE_TEST_TYPE 5
#define NOISE_BLOCKING_TEST_TYPE 6
#define BCAST_TEST_TYPE 7
#define PUT_ONE_TO_ONE_TEST_TYPE 8
#define GET_ONE_TO_ONE_TEST_TYPE 9

#ifdef __cplusplus
extern "C"
{
#endif

extern int all_to_all(Test_time_result_type *times,int mes_length,int num_repeats);
extern int one_to_one(Test_time_result_type *times,int mes_length,int num_repeats);

#ifdef __cplusplus
}
#endif

#ifndef INLINE
    #define INLINE inline
#endif

//const char *default_file_name_prefix = "network";

class Network_speed
{
protected:
    // What we got from file
    enum info_state
    {
        info_state_nothing,
        info_state_no_file, //failed to open
        info_state_processors,
        info_state_test_parameters,
        info_state_partial_matrices,
        info_state_all_done
    } state;

    int comm_size;
    int comm_rank;

    int num_processors;
    int num_messages; // Now it contains number of actually read matrices

    /* Test parameters */
    char test_type_name[100]; // Truncated if too long
    char data_type[256]; // Truncated if too long
    
    int begin_message_length;
    int end_message_length;
    int step_length;
    int noise_message_length;
    int noise_message_num;
    int noise_processors;
    int num_repeats;
    char** host_names;

    char host_name[256];
    int* host_ranks; // Ranks for each processor

    /*
     * The structure with network_test parameters.
     */
    struct network_test_parameters_struct test_parameters;
    /*{
        int  num_procs;
        int  test_type;
        int  begin_message_length;
        int  end_message_length;
        int  step_length;
        int  num_repeats;
        int  noise_message_length;
        int  num_noise_messages;
        int  num_noise_procs;
        const char *file_name_prefix;
    }; */

    /*
     * NetCDF file_id for:
     *  average
     *  median
     *  diviation
     *  minimal values
     *
     */
    int netcdf_file_av;
    int netcdf_file_me;
    int netcdf_file_di;
    int netcdf_file_mi;

    /*
     * NetCDF var_id for:
     *  average
     *  median
     *  diviation
     *  minimal values
     *
     */
    int netcdf_var_av;
    int netcdf_var_me;
    int netcdf_var_di;
    int netcdf_var_mi;

    /*
     * Variables to concentrate test results
     *
     * This is not C++ class but very like.
     */
    Easy_matrix *mtr_av;
    Easy_matrix *mtr_me;
    Easy_matrix *mtr_di;
    Easy_matrix *mtr_mi;

    std::map <int, Easy_matrix*> matrixs;
    int mass_size_of_msg[6];//={10,100,1000,3000,6000,10000}; 

public:
    Network_speed();
    ~Network_speed();
    int fread(char *file_name);
    int make_file(char *file_name);
    //int read_file(char *file_name);
    
    INLINE bool is_no_file( )
    {
        return state == info_state_no_file;
    }
    
    INLINE bool is_processor_info( )
    {
        return state == info_state_processors;
    }
    
    INLINE bool is_test_info( )
    {
        return state >= info_state_test_parameters;
    }
    
    INLINE bool is_any_matrix( )
    {
        return state == info_state_partial_matrices;
    }
    
    INLINE bool is_good_file( )
    {
        return state == info_state_all_done;
    }

    INLINE int get_num_processors( )
    {
        return num_processors;
    }

    INLINE void get_test_type( char* str )
    {
        strcpy( str, test_type_name );
    }

	INLINE void get_data_type( char* str )
	{
		strcpy( str, data_type );
	}
    
    INLINE int get_message_begin_length( )
    {
        return begin_message_length;
    }
    
    INLINE int get_message_end_length( )
    {
        return end_message_length;
    }
    
    INLINE int get_step_length( )
    {
        return step_length;
    }
    
    INLINE int get_noise_message_length( )
    {
        return noise_message_length;
    }
    
    INLINE int get_noise_message_num( )
    {
        return noise_message_num;
    }
    
    INLINE int get_number_of_noise_processors( )
    {
        return noise_processors;
    }
    
    INLINE int get_number_of_repeates( )
    {
        return num_repeats;
    }
    
    INLINE int get_num_messages()
    {
        return num_messages;
    }
    
    /*INLINE int* get_messages_length()
    {
        return messages_length;
    }*/

    INLINE const char* get_host_name(int i)
    {
	return host_names[i];
    }

    INLINE int get_host_rank(int i)
    {
	return host_ranks[i];
    }

    px_my_time_type translate_time(int from,int to,int length);
    int close_and_free();
};

#endif /* __NETWORK_SPEED_H_ */
