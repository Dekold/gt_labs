#include "matrix.h"
#include "string.h"
using namespace std;
int main (int argc, char* argv[])
{
    IO_Params* par = new IO_Params();
    NullBuffer null_buffer;
    ostream* nulls = new ostream(&null_buffer);
    for (int i = 1; i < argc; ++i)
    {
        if (!strcmp(argv[i],"-b")) //Bmatrix mode (else antagonistic)
            par->bmatrix = true;
        if (!strcmp(argv[i],"-v")) //Print debug to cerr (Verbose)
        {
            par->verb = true;
            par->deb_stream = &cerr;
        }
        if (!strcmp(argv[i],"-d")) //Print debug to ofstream (Debug to file)
        {
            if (i < argc - 1) 
            {
                par->fdeb = true;
                string dfilename = argv[i + 1];
                par->deb_stream = new ofstream(dfilename, fstream::out);
            }
        } 
        if (!strcmp(argv[i],"-i")) //Take input from ifstream (File to input)
        {
            if (i < argc - 1) 
            {
                par->finp = true;
                string ifilename = argv[i + 1];
                par->inp_stream = new ifstream(ifilename, fstream::in);
            }
        } 
        if (!strcmp(argv[i],"-o")) //Print output to ofstream (Output to file)
        {
            if (i < argc - 1) 
            {
                par->fout = true;
                string ofilename = argv[i + 1];
                par->out_stream = new ofstream(ofilename, fstream::out);
            }
        } 
        if (!strcmp(argv[i],"-h")) //Print help
        {
            cout << "-v - verbose output (print debug on screen)" << endl;
            cout << "-b - bmatrix mode (if not checked the game considered antagonistic)" << endl;
            cout << "-i <inputfile> - location of file input (if not checked take input from user)" << endl;
            cout << "-o <outputfile> - location of file output (if not checked print int on scrren)" << endl;
            cout << "-d <debugfile> - location of debug output (if not checked either this or verbose do not print it at all)" << endl;
            cout << "-h - help page" << endl;
            delete par;
            return 1;
        } 
    }
    if (!par->verb && !par->fdeb) par->deb_stream = nulls; // If not verbose or debug to file dump it into nullstream
    if (!par->fout) par->out_stream = &cout; //If not using output file use cout instead
    if (!par->finp) par->inp_stream = &cin; // If not using input file take from user
    Matrix* A = new Matrix(par);
    A->Print();    
    A->Mark_SDomStr(); 
    A->Mark_SDomCol();
    A->Mark_WDomStr(); 
    A->Mark_WDomCol();    
    A->Mark_Nash_n_NBA();
    delete A;
    delete par;       
    return 1;
}