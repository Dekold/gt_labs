#include "matrix.h"
using namespace std;

//IO_Params is a class for passing flags and streams to the main class (so you can set params before making a class)
IO_Params::IO_Params(): inp_stream(nullptr), deb_stream(nullptr), out_stream(nullptr), verb(false), finp(false),fout(false),fdeb(false),bmatrix(false)
{}
IO_Params::~IO_Params()
{    
    if (inp_stream != &std::cin)
        delete inp_stream;
    if (deb_stream != &std::cerr)
        delete deb_stream;
    if (out_stream != &std::cout)
        delete out_stream; 
}

//Matrix constructor
Matrix::Matrix(IO_Params* params): par(params)
{ 
    if (!par->finp) cout << "width= "; // if not file input show what we asking for
    *(par->inp_stream) >> sizew;
    if (!par->finp) cout << "height= "; // if not file input show what we asking for
    *(par->inp_stream) >> sizeh; 
    Debug_Call();
    *(par->deb_stream) << "sizew = " << sizew << " sizeh = " << sizeh << endl;
    if (sizew <= 0 || sizeh <= 0) 
    {
        Debug_Call();
        *(par->deb_stream) << "ERROR: bad size input" << endl; 
        error_flag = true;
        return;
    }
    Mat1 = new int*[sizeh];
    for (int i = 0; i < sizeh; ++i) 
        Mat1[i] = new int[sizew];
    Mat2 = new int*[sizeh];
    for (int i = 0; i < sizeh; ++i) 
        Mat2[i] = new int[sizew];
    msdomstr = {};
    msdomcol = {};
    mwdomstr = {};
    mwdomcol = {};
    mnash = {};
    mnbastr = {};
    mnbacol = {};
    for (int i = 0; i < sizeh; ++i)
    {
        for (int j = 0; j < sizew; ++j)
        {
            if (!par->finp) cout << "A1["<< i << "," << j << "]= "; // if not file input show what we asking for
            *(par->inp_stream) >> Mat1[i][j];
            if (par->inp_stream->fail() || par->inp_stream->bad()) 
            {
                Debug_Call();
                *(par->deb_stream) << "ERROR: bad input" << endl; 
                error_flag = true;
                return;
            }
        }
    }
    if (par->bmatrix)   //If bmatrix is set then we continue reading
    {
        for (int i = 0; i < sizeh; ++i)
        {
            for (int j = 0; j < sizew; ++j)
            {
                if (!par->finp) cout << "A2["<< i << "," << j << "]= "; // if not file input show what we asking for
                *(par->inp_stream) >> Mat2[i][j];
                if (par->inp_stream->fail() || par->inp_stream->bad()) 
                {
                    Debug_Call();
                    *(par->deb_stream) << "ERROR: bad input" << endl; 
                    error_flag = true;
                    return;
                }
            }
        }    
    }
    else    //If not consider the game antagonistic
    {
        for (int i = 0; i < sizeh; ++i)
        {
            for (int j = 0; j < sizew; ++j)
            {
                Mat2[i][j] = -1 * Mat1[i][j];
            }
        }
    } 
}

//Handy function to color debug if our deb_stream is cout

void Matrix::Debug_Call()
{
    if (!par->fdeb)
        *(par->deb_stream) << "\033[31mdebug:\033[0m ";
    //else
    //    *(par->deb_stream) << "debug: "; 
    return;
}
//Print context of matrix for both players (mostly for debug)
void Matrix::Print()
{
    if (error_flag) return;
    Debug_Call();
    *(par->deb_stream) << "arrays" << endl;
    for (int i = 0; i < sizeh; ++i)
    {
        for (int j = 0; j < sizew; ++j)
            *(par->out_stream) << Mat1[i][j] << "," << Mat2[i][j] << " ";
        *(par->out_stream) << endl;
    }
    return; 
}

//Mark any Strongly Dominated Strategies for player1
void Matrix::Mark_SDomStr()
{
    if (!msdomstr.empty() || error_flag) return; //If function already outputed data or if we have error somewhere (input) don't do anything
    bool sdom;
    Debug_Call();
    *(par->deb_stream) << "checking strs (player 1)" << endl;
    for (int i = 0; i < sizeh; ++i) // potential sdom str
    {        
        for (int k = 0; k < sizeh; ++k) // other strs of matrix
        {            
            Debug_Call();
            *(par->deb_stream) << "comparing " << i << " and " << k << endl;
            if (i == k) 
            {
                Debug_Call();
                *(par->deb_stream) << "skip - same str" << endl;
                continue; // don't comapre with self
            }
            sdom = true;
            Debug_Call();
            *(par->deb_stream) << "A["<< i <<",]=";
            for (int l = 0; l < sizew; ++l)
            {
                *(par->deb_stream) << Mat1[i][l] << " ";
            }
            *(par->deb_stream) << endl;
            Debug_Call();
            *(par->deb_stream) << "A["<< k <<",]=";
            for (int l = 0; l < sizew; ++l)
            {
                *(par->deb_stream) << Mat1[k][l] << " ";
            }
            *(par->deb_stream) << endl;
            for (int l = 0; l < sizew; ++l) // compare each element of said strs
            {
                
                if (Mat1[i][l] >= Mat1[k][l]) // if one is >= then not sdom so no point in comparing these strs further
                {
                    sdom = false; 
                    Debug_Call();
                    *(par->deb_stream) << "not sdom cause A[" << i << "," << l << "]" << ">=A[" << k << "," << l << "]" << endl;
                    break;
                }
                // otherwise check the next str
            }
            if (sdom) 
            {
                Debug_Call();
                *(par->deb_stream) << "sdom cause everything in "<< i <<" < "<< k << endl;
                msdomstr.push_back(i);    // if all the elements <= then this str is sdom; mark it then stop comparing with other strs
                break; 
            }
        }
    }
    Debug_Call();
    *(par->deb_stream) << "outputing sdomstrs" << endl;
    if (!msdomstr.empty()) 
    {
        *(par->out_stream) << "Found Strongly Dominated Strategies for player 1:" << endl;
        for (int i: msdomstr) // when we are done we can print every marked str
            *(par->out_stream) << "A[," << i << "] ";
        *(par->out_stream) << std::endl;
    }
    else
        *(par->out_stream) << "No Strongly Dominated Strategies for player 1" << endl;  
}

//Mark any Strongly Dominated Strategies for player2
void Matrix::Mark_SDomCol()
{
    if (!msdomcol.empty() || error_flag) return; //If function already outputed data or if we have error somewhere (input) don't do anything
    bool sdom;
    Debug_Call();
    *(par->deb_stream) << "checking cols (player 2)" << endl;
    for (int j = 0; j < sizew; ++j) // potential sdom col
    {        
        for (int k = 0; k < sizew; ++k) // other cols of matrix
        {            
            Debug_Call();
            *(par->deb_stream) << "comparing " << j << " and " << k << endl;
            if (j == k) 
            {
                Debug_Call();
                *(par->deb_stream) << "skip - same col" << endl;
                continue; // don't comapre with self
            }
            sdom = true;
            Debug_Call();
            *(par->deb_stream) << "A[,"<< j <<"]=";
            for (int l = 0; l < sizeh; ++l)
            {
                *(par->deb_stream) << Mat2[l][j] << " ";
            }
            *(par->deb_stream) << endl;
            Debug_Call();
            *(par->deb_stream) << "A[,"<< k <<"]=";
            for (int l = 0; l < sizeh; ++l)
            {
                *(par->deb_stream) << Mat2[l][k] << " ";
            }
            *(par->deb_stream) << endl;
            for (int l = 0; l < sizeh; ++l) // compare each element of said cols
            {                
                if (Mat2[l][j] >= Mat2[l][k]) // if one is >= then not sdom so no point in comparing these cols further
                {
                    sdom = false; 
                    Debug_Call();
                    *(par->deb_stream) << "not sdom cause A[" << l << "," << j << "]" << ">=A[" << l << "," << k << "]" << endl;
                    break;
                }
                // otherwise check the next string
            }
            if (sdom) 
            {
                Debug_Call();
                *(par->deb_stream) << "sdom cause everything in "<< j <<" < "<< k << endl;
                msdomcol.push_back(j);    // if all the elements <= then this string is sdom; mark it then stop comparing with other strings
                break; 
            }
        }
    }
    Debug_Call();
    *(par->deb_stream) << "outputing sdomcols" << endl;
    if (!msdomcol.empty() || error_flag) 
    {
        *(par->out_stream) << "Found Strongly Dominated Strategies for player 2:" << endl;
        for (int j: msdomcol) // when we are done we can print every marked col;
            *(par->out_stream) << "A[" << j << ",] ";
        *(par->out_stream) << std::endl;
    }
    else
        *(par->out_stream) << "No Strongly Dominated Strategies for player 2" << endl;
    return; 
}

//Mark any Weakly Dominated Strategies for player1
void Matrix::Mark_WDomStr()
{
    if (!mwdomcol.empty() || error_flag) return; //If function already outputed data or if we have error somewhere (input) don't do anything
    bool wdom;
    Debug_Call();
    *(par->deb_stream) << "checking strs (player 1)" << endl;
    for (int i = 0; i < sizeh; ++i) // potential wdom str
    {        
        for (int k = 0; k < sizeh; ++k) // other strs of matrix
        { 
            Debug_Call();           
            *(par->deb_stream) << "comparing " << i << " and " << k << endl;
            if (i == k) 
            {
                Debug_Call();
                *(par->deb_stream) << "skip - same str" << endl;
                continue; // don't comapre with self
            }
            wdom = true;
            Debug_Call();
            *(par->deb_stream) << "A["<< i <<",]=";
            for (int l = 0; l < sizew; ++l)
            {
                *(par->deb_stream) << Mat1[i][l] << " ";
            }
            *(par->deb_stream) << endl;
            Debug_Call();
            *(par->deb_stream) << "A["<< k <<",]=";
            for (int l = 0; l < sizew; ++l)
            {
                *(par->deb_stream) << Mat1[k][l] << " ";
            }
            *(par->deb_stream) << endl;
            for (int l = 0; l < sizew; ++l) // compare each element of said strs
            {
                
                if (Mat1[i][l] > Mat1[k][l]) // if one is > then not wdom so no point in comparing these strs further
                {
                    wdom = false; 
                    Debug_Call();
                    *(par->deb_stream) << "not wdom cause A[" << i << "," << l << "]" << ">A[" << k << "," << l << "]" << endl;
                    break;
                }
                // otherwise check the next str
            }
            if (wdom) 
            {
                Debug_Call();
                *(par->deb_stream) << "wdom cause everything in "<< i <<" < "<< k << endl;
                mwdomstr.push_back(i);    // if all the elements <= then this str is wdom; mark it then stop comparing with other strs
                break; 
            }
        }
    }
    Debug_Call();
    *(par->deb_stream) << "outputing wdomstrs" << endl;
    if (!mwdomstr.empty()) 
    {
        *(par->out_stream) << "Found Weakly Dominated Strategies for player 1:" << endl;
        for (int i: mwdomstr) // when we are done we can print every marked str
            *(par->out_stream) << "A[," << i << "] ";
        *(par->out_stream) << std::endl;
    }
    else
        *(par->out_stream) << "No Weakly Dominated Strategies for player 1" << endl;  
}

//Mark any Weakly Dominated Strategies for player2
void Matrix::Mark_WDomCol()
{
    if (!mwdomcol.empty() || error_flag) return; //If function already outputed data or if we have error somewhere (input) don't do anything
    bool wdom;
    Debug_Call();
    *(par->deb_stream) << "checking cols (player 2)" << endl;
    for (int j = 0; j < sizew; ++j) // potential wdom col
    {        
        for (int k = 0; k < sizew; ++k) // other cols of matrix
        {     
            Debug_Call();       
            *(par->deb_stream) << "comparing " << j << " and " << k << endl;
            if (j == k) 
            {
                Debug_Call();
                *(par->deb_stream) << "skip - same col" << endl;
                continue; // don't comapre with self
            }
            wdom = true;
            Debug_Call();
            *(par->deb_stream) << "A[,"<< j <<"]=";
            for (int l = 0; l < sizeh; ++l)
            {
                *(par->deb_stream) << Mat2[l][j] << " ";
            }
            *(par->deb_stream) << endl;
            Debug_Call();
            *(par->deb_stream) << "A[,"<< k <<"]=";
            for (int l = 0; l < sizeh; ++l)
            {
                *(par->deb_stream) << Mat2[l][k] << " ";
            }
            *(par->deb_stream) << endl;
            for (int l = 0; l < sizeh; ++l) // compare each element of said cols
            {                
                if (Mat2[l][j] > Mat2[l][k]) // if one is > then not wdom so no point in comparing these cols further
                {
                    wdom = false; 
                    Debug_Call();
                    *(par->deb_stream) << "not wdom cause A[" << l << "," << j << "]" << ">=A[" << l << "," << k << "]" << endl;
                    break;
                }
                // otherwise check the next string
            }
            if (wdom) 
            {
                Debug_Call();
                *(par->deb_stream) << "wdom cause everything in "<< j <<" < "<< k << endl;
                mwdomcol.push_back(j);    // if all the elements <= then this string is wdom; mark it then stop comparing with other strings
                break; 
            }
        }
    }
    Debug_Call();
    *(par->deb_stream) << "outputing wdomcols" << endl;
    if (!mwdomcol.empty()) 
    {
        *(par->out_stream) << "Found Weakly Dominated Strategies for player 2:" << endl;
        for (int j: mwdomcol) // when we are done we can print every marked col;
            *(par->out_stream) << "A[" << j << ",] ";
        *(par->out_stream) << std::endl;
    }
    else
        *(par->out_stream) << "No Weakly Dominated Strategies for player 2" << endl;
    return; 
}

// both Nash Equilibrium and Never Best Answers require finding best answers so it makes sence to merge them
void Matrix::Mark_Nash_n_NBA() 
{
    if (!mnash.empty() || !mnbastr.empty() || !mnbacol.empty() || error_flag) return; //If function already outputed data or if we have error somewhere (input) don't do anything
    bool** ans1, **ans2; // matrix of best answers for each player (true - best, false - not best)
    bool not_nba; // nba flag
    ans1 = new bool*[sizeh];
    for (int i = 0; i < sizeh; ++i) 
        ans1[i] = new bool[sizew];
    for (int i = 0; i < sizeh; ++i)
        for (int j = 0; j < sizew; ++j)
            ans1[i][j] = false;
    ans2 = new bool*[sizeh];
    for (int i = 0; i < sizeh; ++i) 
        ans2[i] = new bool[sizew];
    for (int i = 0; i < sizeh; ++i)
        for (int j = 0; j < sizew; ++j)
            ans2[i][j] = false;
    int max_val;
    Debug_Call();
    *(par->deb_stream) << "finding best answers for player 1" << endl;
    for (int j = 0; j < sizew; ++j) // choise of our opponent (player 2)
    {     
        max_val = Mat1[0][j];
        ans1[0][j] = true;
        for (int i = 1; i < sizeh; ++i) // our choise (finding max in col)
        { 
            Debug_Call();
            *(par->deb_stream) << "current A[" << i << "," << j << "]=" << Mat1[i][j] << endl; 
            if (Mat1[i][j] > max_val) // compare with current maximum if more then reset maximum if equal then just add another marker
            {  
                for (int k = 0; k < i; ++k) // reset markers before currrent
                    ans1[k][j] = false;
                max_val = Mat1[i][j];
                ans1[i][j] = true;
                Debug_Call();
                *(par->deb_stream) << "new max found: A[" << i << "," << j << "]=" << max_val << endl;
                continue;
            }
            if (Mat1[i][j] == max_val)
            {
                ans1[i][j] = true;
                Debug_Call();
                *(par->deb_stream) << "value = to max found: A[" << i << "," << j << "]=" << max_val << endl;
            }
        }            
    }
    Debug_Call();
    *(par->deb_stream) << "finding best answers for player 2" << endl;
    for (int i = 0; i < sizeh; ++i) // choise of our opponent (player 1)
    {     
        max_val = Mat2[i][0];
        ans2[i][0] = true;
        for (int j = 1; j < sizew; ++j) // our choise (finding max in string)
        { 
            Debug_Call();
            *(par->deb_stream) << "current A[" << i << "," << j << "]=" << Mat2[i][j] << endl; 
            if (Mat2[i][j] > max_val) // compare with current maximum if more then reset maximum if equal then just add another marker
            {  
                for (int k = 0; k < j; ++k) // reset markers before currrent
                    ans2[i][k] = false;
                max_val = Mat2[i][j];
                ans2[i][j] = true;
                Debug_Call();
                *(par->deb_stream) << "new max found: A[" << i << "," << j << "]=" << max_val << endl;
                continue;
            }
            if (Mat2[i][j] == max_val)
            {
                ans2[i][j] = true;
                Debug_Call();
                *(par->deb_stream) << "value = to max found: A[" << i << "," << j << "]=" << max_val << endl;
            }
        }            
    }
    Debug_Call();
    *(par->deb_stream) << "best answers for player1:" << endl;
    for (int i = 0; i < sizeh; ++i)
    {
        for (int j = 0; j < sizew; ++j)
            *(par->deb_stream) << ans1[i][j] << " ";
        *(par->deb_stream) << endl;
    }
    Debug_Call();
    *(par->deb_stream) << "best answers for player2:" << endl;
    for (int i = 0; i < sizeh; ++i)
    {
        for (int j = 0; j < sizew; ++j)
            *(par->deb_stream) << ans2[i][j] << " ";
        *(par->deb_stream) << endl;
    }
    Debug_Call();
    *(par->deb_stream) << "finding Never Best Answers for player1" << endl; // search for NBA    
    for (int i = 0; i < sizeh; ++i) // for each str
    {
        not_nba = false; // initially each str considered nba
        for (int j = 0; j < sizew; ++j)
        {
            Debug_Call();
            *(par->deb_stream) << "ans1[" << i << "," << j << "]=" << ans1[i][j] << endl;
            not_nba = not_nba || ans1[i][j];  // binary or with each member of that string to find if it's nba or not 
            if (not_nba) 
            { 
                Debug_Call();
                *(par->deb_stream) << "str A[" << i << ",] is not an nba cause ans1[" << i << "," << j << "]=1" << endl;
                break;
            }         
        }
        if (!not_nba) 
        {
            mnbastr.push_back(i);
            Debug_Call();
            *(par->deb_stream) << "str A[" << i << ",] is an nba" << endl;
        }
    } 
    Debug_Call();
    *(par->deb_stream) << "outputing NBA for player 1" << endl;
    if (!mnbastr.empty()) 
    {
        *(par->out_stream) << "Found Never Best Answers for player 1:" << endl;
        for (int i: mnbastr) // when we are done we can print every NBA;
            *(par->out_stream) << "A[" << i  << ",]" << endl;
    }
    else
        *(par->out_stream) << "No Never Best Answers for player 1 found" << endl; 
    Debug_Call();
    *(par->deb_stream) << "finding Never Best Answers for player2" << endl; // search for NBA    
    for (int j = 0; j < sizew; ++j) // for each col
    {
        not_nba = false; // initially each col considered nba
        for (int i = 0; i < sizeh; ++i)
        {
            Debug_Call();
            *(par->deb_stream) << "ans2[" << i << "," << j << "]=" << ans2[i][j] << endl;
            not_nba = not_nba || ans2[i][j];  // binary or with each member of that string to find if it's nba or not 
            if (not_nba) 
            { 
                Debug_Call();
                *(par->deb_stream) << "col A[," << j << "] is not an nba cause ans2[" << i << "," << j << "]=1" << endl;
                break;
            }         
        }
        if (!not_nba) 
        {
            mnbacol.push_back(j);
            Debug_Call();
            *(par->deb_stream) << "col A[," << j << "] is an nba" << endl;
        }
    } 
    Debug_Call();
    *(par->deb_stream) << "outputing NBA for player 2" << endl;
    if (!mnbacol.empty()) 
    {
        *(par->out_stream) << "Found Never Best Answer(s) for player 2:" << endl;
        for (int j: mnbacol) // when we are done we can print every NBA;
            *(par->out_stream) << "A[," << j  << "]" << endl;
    }
    else
        *(par->out_stream) << "No Never Best Answer(s) for player 2 found" << endl; 
    Debug_Call();
    *(par->deb_stream) << "finding nash" << endl; // search for nash
    for (int i = 0; i < sizeh; ++i)
        for (int j = 0; j < sizew; ++j)
            if (ans1[i][j] && ans2[i][j])  // if best answers in a same spot then it's nash
            {
                Debug_Call();
                *(par->deb_stream) << "nash found - A[" << i << "][" << j << "]=" << Mat1[i][j] << "," << Mat2[i][j] << " adding to collection" << endl;
                mnash.push_back(i); 
                mnash.push_back(j);
            }
    Debug_Call();
    *(par->deb_stream) << "outputing nash" << endl;
    if (!mnash.empty()) 
    {
        *(par->out_stream) << "Found Nash Equlibrium(s):" << endl;
        for (int i = 0; i < mnash.size(); i = i + 2) // when we are done we can print every nash;
            *(par->out_stream) << "A[" << mnash[i]  << "][" << mnash[i+1]  << "]=" << Mat1[mnash[i]][mnash[i + 1]] << "," << Mat2[mnash[i]][mnash[i + 1]] << " ";
        *(par->out_stream) << std::endl;
    }
    else
        *(par->out_stream) << "No nash found" << endl;    
    for (int i = 0; i < sizeh; ++i) 
        delete[] ans1[i];
    delete[] ans1;
    for (int i = 0; i < sizeh; ++i) 
        delete[] ans2[i];
    delete[] ans2;
    return; 
}

//Destructor of Main Class
Matrix::~Matrix()
{
    Debug_Call();
    *(par->deb_stream) << "array wipe" << endl;
    for (int i = 0; i < sizeh; ++i) 
        delete[] Mat1[i];
    delete[] Mat1;
    for (int i = 0; i < sizeh; ++i) 
        delete[] Mat2[i];
    delete[] Mat2;
    Debug_Call();
    *(par->deb_stream) << "stream wipe" << endl;
}