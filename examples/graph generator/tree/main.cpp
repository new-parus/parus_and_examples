#include <iostream>
#include <fstream>

using namespace std;

ofstream graph;
int number_of_nodes;

void printnode(int number, int from, int to)
{
	int i=1, layer=0;
	while (i<number+1) 
	{
		i=i*2;
		layer+=1;
	}
	graph<<"    <NODE_BEGIN>"<<endl;
    graph<<"        number "<<number<<endl;
    graph<<"        type 1"<<endl;
    graph<<"        weight 100"<<endl;
    graph<<"        layer "<<layer<<endl;
    graph<<"        num_input_edges 1"<<endl;
    graph<<"        edges ( "<<number<<" )"<<endl;
    if (to==0)
    {
    	graph<<"        num_output_edges 0"<<endl;
    	graph<<"        edges (  )"<<endl;
    }else{
		graph<<"        num_output_edges 2"<<endl;
    	graph<<"        edges ( "<<to<<" "<<to+1<<" )"<<endl;
    }
    graph<<"        head \"head.grfcpp\""<<endl;
    graph<<"        body \"body.grfcpp\""<<endl;
    graph<<"        tail \"tail.grfcpp\""<<endl;
    graph<<"    <NODE_END>"<<endl<<endl;
}

void printedge(int number, int from, int to)
{
    graph<<"    <EDGE_BEGIN>"<<endl;
    graph<<"       number "<<number<<endl;
    graph<<"       weight 100"<<endl;
    graph<<"       type   GRAPH_NONE"<<endl;
    graph<<"       num_var 1"<<endl;
    graph<<"       num_send_nodes 1"<<endl;
    graph<<"       send_nodes ( "<<from<<" )"<<endl;
    graph<<"       num_recv_nodes 2"<<endl;
    graph<<"       recv_nodes ( "<<to<<" )"<<endl;
    graph<<"            <SEND_BEGIN>"<<endl;
    graph<<"                <CHUNK_BEGIN>"<<endl;
    graph<<"                   name \"row\""<<endl;
    graph<<"                   type GRAPH_DOUBLE"<<endl;
    graph<<"                   left_offset \"0\""<<endl;
    graph<<"                   right_offset \""<<500*sizeof(double)<<"+F_LEN\""<<endl;
    graph<<"                <CHUNK_END>"<<endl;
    graph<<"            <SEND_END>"<<endl;
    graph<<"            <RECIEVE_BEGIN>"<<endl;
    graph<<"                <CHUNK_BEGIN>"<<endl;
    graph<<"                   name \"row\""<<endl;
    graph<<"                   type GRAPH_DOUBLE"<<endl;
    graph<<"                   left_offset \"0\""<<endl;
    graph<<"                   right_offset \""<<500*sizeof(double)<<"\""<<endl;
    graph<<"                <CHUNK_END>"<<endl;
    graph<<"            <RECIEVE_END>"<<endl;
    graph<<"     <EDGE_END>"<<endl<<endl;
}


int main()
{
    graph.open("graph.grf");
    number_of_nodes=513;

    graph<<"<GRAPH_BEGIN>"<<endl;
    graph<<"    header \"\""<<endl;
    graph<<"    root \"\""<<endl;
    graph<<"    tail \"\""<<endl;
    graph<<"    num_nodes "<<number_of_nodes<<endl<<endl;


    graph<<"<NODES_BEGIN>"<<endl;
    
    int j=2;
    for (int i=1; i<number_of_nodes+1; i++)
    {
        int from = (int)(i/2) - (i%2);
        int to = i*2;
        printnode(i,from,to);
    }

    graph<<"<NODES_END>"<<endl<<endl;

    for (int i=1; i<number_of_nodes+1; i++)
    {
        int from = (int)(i/2) - (i%2);
        int to = i*2;
        if (to+1 <= number_of_nodes)
        { 
            j++;
            j++;
        }else{
            if (to == number_of_nodes)
            {
                j++;
            }else{to=0;}
        }
    }

    graph<<"    num_edges "<< j <<endl<<endl;
    j=2;
    graph<<"<EDGES_BEGIN>"<<endl;
    for (int i=1; i<number_of_nodes+1; i++)
    {
        int from = (int)(i/2) - (i%2);
        int to = i*2;
        if (to+1 <= number_of_nodes)
        { 
            printedge(j,i,to);
            j++;
            printedge(j,i,to+1);
            j++;
        }else{
            if (to == number_of_nodes)
            {
                printedge(j,i,to);
                j++;
            }else{to=0;}
        }
    }
    graph<<"<EDGES_END>"<<endl;
    return 0;
}
