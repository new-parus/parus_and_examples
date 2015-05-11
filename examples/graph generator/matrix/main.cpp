#include <iostream>
#include <fstream>
#include <list>
#include <algorithm>
#include <vector>
#include "SparseMatrix.h"

using namespace std;

ofstream node;
ofstream edge;
int number_of_nodes;

void printnode(int number, int from, int to)
{
	int i=1, layer=0;
	while (i<number+1) 
	{
		i=i*2;
		layer+=1;
	}
	node<<"    <NODE_BEGIN>"<<endl;
    node<<"        number "<<number<<endl;
    node<<"        type 1"<<endl;
    node<<"        weight 100"<<endl;
    node<<"        layer "<<layer<<endl;
    node<<"        num_input_edges 1"<<endl;
    node<<"        edges ( "<<number<<" )"<<endl;
    if (to==0)
    {
    	node<<"        num_output_edges 0"<<endl;
    	node<<"        edges (  )"<<endl;
    }else{
		node<<"        num_output_edges 2"<<endl;
    	node<<"        edges ( "<<to<<" "<<to+1<<" )"<<endl;
    }
    node<<"        head \"head\""<<endl;
    node<<"        body \"node\""<<endl;
    node<<"        tail \"\""<<endl;
    node<<"    <NODE_END>"<<endl<<endl;
}

void printedge(int number, int from, int to)
{
    edge<<"    <EDGE_BEGIN>"<<endl;
    edge<<"       number "<<number<<endl;
    edge<<"       weight 100"<<endl;
    edge<<"       type   GRAPH_NONE"<<endl;
    edge<<"       num_var 8"<<endl;
    edge<<"       num_send_nodes 1"<<endl;
    edge<<"       send_nodes ( "<<from<<" )"<<endl;
    edge<<"       num_recv_nodes 2"<<endl;
    edge<<"       recv_nodes ( "<<to<<" )"<<endl;
    edge<<"            <SEND_BEGIN>"<<endl;
    edge<<"                <CHUNK_BEGIN>"<<endl;
    edge<<"                   name \"row\""<<endl;
    edge<<"                   type GRAPH_LONG"<<endl;
    edge<<"                   left_offset \"0\""<<endl;
    edge<<"                   right_offset \""<<5*sizeof(long)<<"+F_LEN\""<<endl;
    edge<<"                <CHUNK_END>"<<endl;
    edge<<"            <SEND_END>"<<endl;
    edge<<"            <RECIEVE_BEGIN>"<<endl;
    edge<<"                <CHUNK_BEGIN>"<<endl;
    edge<<"                   name \"row\""<<endl;
    edge<<"                   type GRAPH_LONG"<<endl;
    edge<<"                   left_offset \"0\""<<endl;
    edge<<"                   right_offset \""<<5*sizeof(long)<<"\""<<endl;
    edge<<"                <CHUNK_END>"<<endl;
    edge<<"            <RECIEVE_END>"<<endl;
    edge<<"     <EDGE_END>"<<endl<<endl;
}
//не забыть возврат из функции.
void printBeginNode()
{
    node<<"<NODE_BEGIN>"<<endl;
    node<<"    number "<<0<<endl;
    node<<"    type 0"<<endl;
    node<<"    weight 100"<<endl;
    node<<"    layer 2"<<endl;
    node<<"    num_input_edges 0"<<endl;
    node<<"    edges (  )"<<endl;
    node<<"    num_output_edges 1"<<endl;
    node<<"    edges ( 1 )"<<endl;
    node<<"    head \"\""<<endl;
    node<<"    body \"BigNode\""<<endl;
    node<<"    tail \"\""<<endl;
    node<<"<NODE_END>"<<endl<<endl;
}

void PrintGraph( SparseMatix& mat1, SparseMatix& mat2)
{
    //otv.SetMaxCol(mat2.GetMaxCol());
    //otv.SetMaxRow(mat1.GetMaxRow());
    for (long i=0;i<mat1.GetMaxRow();i++)
    {
        long r_begin=0,r_end=0;
        mat1.GetRow(i,r_begin,r_end);
        if ( (r_begin==0)&&(r_end==0) )
            continue;
        for (long j=0;j<mat2.GetMaxCol();j++)
        {
            long c_begin=0,c_end=0;
            mat2.GetCol(j,c_begin,c_end);
            //cout<<j<<c_begin<<c_end<<endl;
            if ( (c_begin==0)&&(c_end==0) )
                continue;
            ///Mult; get mat(i,j)
            double elem=0;
            double* element_mass1=new double[r_end-r_begin];
            double* element_mass2=new double[c_end-c_begin];
            long *row=new long[c_end-c_begin];
            long *col=new long[r_end-r_begin];
            mat1.CopyPartOfCol(r_begin,r_end,col,element_mass1);
            mat2.CopyPartOfRow(c_begin,c_end,row,element_mass2);
            printgrf(row,col,element_mass1,element_mass2,i,j,r_size, c_size);



            for(long k=0; k<(r_end-r_begin); k++)
            {
                long k2=0;
                while ( ( ((int)col[k])<((int)row[k2]) ) && (k2<(c_end-c_begin) ) )
                    k2++;
                while ( ( ((int)col[k])>((int)row[k2]) ) && (k2<(c_end-c_begin) ) )
                   k2++;
                if (k2==(c_end-c_begin)) continue;
                if ((int)col[k]==(int)row[k2])
                    elem+=(element_mass1[k])*(element_mass2[k2]);
            }
            if (elem!=0)
            {
                //cout<<"add "<<i<<" "<<j<<" "<<elem<<endl;
                otv.AddElement(i,j,elem);
            }
            delete[] row;
            delete[] col;
            delete[] element_mass1;
            delete[] element_mass2;
        }

    }
    //return otv;
}




//494_bus.mtx
int main()
{
    node.open("node.grf");
    edge.open("edge.grf");
    SparseMatix mat1, mat2;
    mat1.CreateMatrix("/home/grin/cpp/Matrix_Parus/bin/Debug/1.txt");
    mat1.SortForRow();
    //mat1.PrintMatrix2();
    mat2.CreateMatrix("/home/grin/cpp/Matrix_Parus/bin/Debug/2.txt");
    mat2.SortForCol();
    //mat2.PrintMatrix2();
    if (mat2.GetMaxCol()!=mat1.GetMaxRow())
    {
        cout<<"Количество столбцов первой матрицы не совпадает с количеством строк второй."<<endl;
        return 1;
    }
    PrintGraph(mat1, mat2);
    //otv.PrintMatrix("/home/grin/cpp/Matrix_Parus/bin/Debug/otv.mtx");
    return 0;
}