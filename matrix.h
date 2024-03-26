#include <iostream>
#include <fstream>
#include <string>
#include <vector>
//IO_Params is a class for passing flags and streams to the main class (so you can set params before making a class)
class IO_Params
{
    public:
        bool verb, finp, fout, fdeb, bmatrix;
        std::string ifilename, ofilename, dfilename;
        std::istream* inp_stream;
        std::ostream* deb_stream, *out_stream;
        IO_Params();
        ~IO_Params();
};
//Matrix is a Main Class holding values of matrix game and bethods to solve it
class Matrix
{
    protected:
        int** Mat1;
        int** Mat2;
        int sizew, sizeh;
        IO_Params* par;
        std::vector<int> msdomstr, msdomcol, mwdomstr, mwdomcol, mnash, mnbastr, mnbacol;
        bool error_flag;
    public:
        Matrix(IO_Params* params);
        void Debug_Call();
        void Mark_SDomStr();
        void Mark_SDomCol();
        void Mark_WDomStr();
        void Mark_WDomCol();
        void Mark_Nash_n_NBA();
        void Print();
        ~Matrix();
};
class NullBuffer : public std::streambuf
{
    public:
        int overflow(int c) { return c; }
};