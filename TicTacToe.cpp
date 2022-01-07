#include<iostream>
using namespace std;
struct Move{
        int row,col;
        Move(int x,int y){
            row=x;
            col=y;
        }
    };
class TicTacToe{

    //0 means not taken
    //1 means taken by player 1
    //2 means taken by player 2
    int player_board1[25];
    //0 means turn of the player 1(minimizer)
    //1 means turn of the player 2(maximizer)(AI)
    int AIturn =0,leftFree=25;
    std::list<int> minimaxTree;
    public:
        TicTacToe(){
            for(int i=0;i<25;i++)
                player_board1[i]=0;
        };
        void printBoard(){
            std::cout<<"x\\y| ";
            for(int i=1;i<6;i++){
                std::cout<<i<<" | ";
            }
            std::cout<<"\n";
            for(int i=0;i<5;i++){
                std::cout<<i+1<<"  |";
                for(int j=0;j<5;j++){
                    if(player_board1[i*5+j]==1){
                        std::cout<<" x |";
                    }
                    else if (player_board1[i*5+j]==2){
                        std::cout<<" o |";
                    }
                    else{
                        std::cout<<"   |";
                    }
                }
                std::cout<<"\n";
            }
        }
        int evaluate2(){
            //if there is x in a row give it x(or -x)
            int prev=0;
            int inARow=0;
            int score=0;
            int multipliers[]={1,2,8,32,256};
            //checking rows
            for(int column=0;column<5;column++){
                prev=0;
                inARow=0;
                for(int row=0;row<5;row++){
                    switch(player_board1[column*5+row]){
                    case 0:
                        inARow=0;
                        prev=0;
                        break;
                    case 1:
                        if(prev==1){
                            inARow+=1;
                        }
                        else{
                            inARow=0;
                            prev=1;
                        }
                        score-=multipliers[inARow];
                        break;
                    case 2:
                        if(prev==2){
                            inARow+=1;
                        }
                        else{
                            inARow=0;
                            prev=2;
                        }
                        score+=multipliers[inARow];
                        break;
                    }
                    if(inARow==4){
                        printBoard();
                        std::cout<<"WE HAVE A WINNER !"<<prev<<" has won (1-player, 2-AI).\n";
                        return 1;
                    }
                }
            }
            prev=0;
            inARow=0;
            //checking columns
            for(int row=0;row<5;row++){
                prev=0;
                inARow=0;
                for(int column=0;column<5;column++){
                    switch(player_board1[column*5+row]){
                        case 0:
                            inARow=0;
                            prev=0;
                            break;
                        case 1:
                            if(prev==1){
                                inARow+=1;
                            }
                            else{
                                inARow=0;
                                prev=1;
                            }
                            score-=multipliers[inARow];
                            break;
                        case 2:
                            if(prev==2){
                                inARow+=1;
                            }
                            else{
                                inARow=0;
                                prev=2;
                            }
                            score+=multipliers[inARow];
                            break;
                    }
                    if(inARow==4){
                        printBoard();
                        std::cout<<"WE HAVE A WINNER !"<<prev<<" has won (1-player, 2-AI).\n";
                        return 1;
                    }
                }
            }
            prev=0;
            inARow=0;
            //checking diagonals
            for(int row=0;row<5;row++){
                switch(player_board1[row*5+row]){
                        case 0:
                            inARow=0;
                            prev=0;
                            break;
                        case 1:
                            if(prev==1){
                                inARow+=1;
                            }
                            else{
                                inARow=0;
                                prev=1;
                            }
                            score-=multipliers[inARow];
                            break;
                        case 2:
                            if(prev==2){
                                inARow+=1;
                            }
                            else{
                                inARow=0;
                                prev=2;
                            }
                            score+=multipliers[inARow];
                            break;
                    }
                if(inARow==4){
                    printBoard();
                    std::cout<<"WE HAVE A WINNER !"<<prev<<" has won (1-player, 2-AI).\n";
                    return 1;
                }
            }
            prev=0;
            inARow=0;
            int row=0;
            for(int column=4;column>=0;column--){
                switch(player_board1[row*5+column]){
                        case 0:
                            inARow=0;
                            prev=0;
                            break;
                        case 1:
                            if(prev==1){
                                inARow+=1;
                            }
                            else{
                                inARow=0;
                                prev=1;
                            }
                            score-=multipliers[inARow];
                            break;
                        case 2:
                            if(prev==2){
                                inARow+=1;
                            }
                            else{
                                inARow=0;
                                prev=2;
                            }
                            score+=multipliers[inARow];
                            break;
                }
                row++;
                if(inARow==4){
                        printBoard();
                        std::cout<<"WE HAVE A WINNER !"<<prev<<" has won (1-player, 2-AI).\n";
                    return 1;
                }
            }
            return 0;
        }
        bool gameOver(){
            if(evaluate2()){
                return true;
            }
            if(leftFree>0){
                return false;
            }
            else{
                return true;
            }
        }
        int maximum(int a,int b){
            return a>b?a:b;
        }
        int minimum(int a,int b){
            return a<b?a:b;
        }
        //returns a value with values assigned depending on the number of x/o in a row
        int evaluate(){
            //if there is x in a row give it x(or -x)
            int prev=0;
            int inARow=0;
            int score=0;
            int multipliers1[]={1,2,4,16,32};
            int multipliers2[]={1,2,4,8,16};
            int penalty=32;
            //checking rows
            for(int column=0;column<5;column++){
                prev=0;
                inARow=0;
                for(int row=0;row<5;row++){
                    switch(player_board1[column*5+row]){
                    //calculate how many we have in a row
                    case 0:
                        inARow=0;
                        prev=0;
                        break;
                    case 1:
                        if(prev==1){
                            inARow+=1;
                        }
                        else if (prev==2){
                            inARow=0;
                            prev=1;
                        }
                        else{
                            inARow=0;
                            prev=1;
                        }
                        score-=multipliers1[inARow];
                        break;
                    case 2:
                        if(prev==2){
                            inARow+=1;
                        }
                        else if (prev==2){
                            inARow=0;
                            prev=1;

                        }
                        else{
                            inARow=0;
                            prev=1;
                        }
                        score+=multipliers2[inARow];
                        break;
                    }
                }
            }
            prev=0;
            inARow=0;
            //checking columns
            for(int row=0;row<5;row++){
                prev=0;
                inARow=0;
                for(int column=0;column<5;column++){
                    switch(player_board1[column*5+row]){
                    case 0:
                        inARow=0;
                        prev=0;
                        break;
                    case 1:
                        if(prev==1){
                            inARow+=1;
                        }
                        else if (prev==2){
                            inARow=0;
                            prev=1;
                        }
                        else{
                            inARow=0;
                            prev=1;
                        }
                        score-=multipliers1[inARow];
                        break;
                    case 2:
                        if(prev==2){
                            inARow+=1;
                        }
                        else if (prev==2){
                            inARow=0;
                            prev=1;

                        }
                        else{
                            inARow=0;
                            prev=1;
                        }
                        score+=multipliers2[inARow];
                        break;
                    }
                }
            }
            prev=0;
            inARow=0;
            //checking diagonals
            for(int row=0;row<5;row++){
                switch(player_board1[row*5+row]){
                    case 0:
                        inARow=0;
                        prev=0;
                        break;
                    case 1:
                        if(prev==1){
                            inARow+=1;
                        }
                        else if (prev==2){
                            inARow=0;
                            prev=1;

                        }
                        else{
                            inARow=0;
                            prev=1;
                        }
                        score-=multipliers1[inARow];
                        break;
                    case 2:
                        if(prev==2){
                            inARow+=1;
                        }
                        else if (prev==2){
                            inARow=0;
                            prev=1;

                        }
                        else{
                            inARow=0;
                            prev=1;
                        }
                        score+=multipliers1[inARow];
                        break;
                    }
            }
            prev=0;
            inARow=0;
            int column=0;
            for(int row=4;row>=0;row--){
               switch(player_board1[column*5+row]){
                    case 0:
                        inARow=0;
                        prev=0;
                        break;
                    case 1:
                        if(prev==1){
                            inARow+=1;
                        }
                        else if (prev==2){
                            inARow=0;
                            prev=1;

                        }
                        else{
                            inARow=0;
                            prev=1;
                        }
                        score-=multipliers1[inARow];
                        break;
                    case 2:
                        if(prev==2){
                            inARow+=1;
                        }
                        else if (prev==2){
                            inARow=0;
                            prev=1;

                        }
                        else{
                            inARow=0;
                            prev=1;
                        }
                        score+=multipliers2[inARow];
                        break;
                    }
                column++;
            }
            return score;
        }
        int minimax(int depth,int isMaxPlayer){
            int score=evaluate();
            if(depth==0){

//                cout<<"\nDepth:"<<depth<<" Best:"<<score<<"\n";
                return score;
            }
            if(isMaxPlayer){
                int bestVal=-1000;
                for(int i=0;i<25;i++){
                    if(player_board1[i]==0){
                        player_board1[i]=2;
                        int tempVal = minimax(depth-1,false);
                        bestVal = maximum(bestVal,tempVal);
                        player_board1[i]=0;
                    }
                }
       //         std::cout<<"\nDepth:"<<depth<<" Best"<<bestVal<<"\n";
                return bestVal;
            }
            else{
                int bestVal=1000;
                for(int i=0;i<25;i++){
                    if(player_board1[i]==0){
                        player_board1[i]=1;
                        int tempVal = minimax(depth-1,true);
                        bestVal = minimum(bestVal,tempVal);
                        player_board1[i]=0;
                    }
                }
               // std::cout<<"\nDepth:"<<depth<<" Best"<<bestVal<<"\n";
                return bestVal;
            }
        }
        Move findBestMove(){
            int bestVal= -1000;
            Move bestMove=Move(-1,-1);
            for(int i=0;i<5;i++){
                for(int j=0;j<5;j++){
                    if(player_board1[i*5+j]==0){
                        player_board1[i*5+j]=2;
                       // std::cout<<"MiniMax STARTED !"
                        int moveVal=minimax(4,true);
                        player_board1[i*5+j]=0;
                        if(moveVal>bestVal){
                            bestMove.row=j;
                            bestMove.col=i;
                            bestVal=moveVal;
                        }
                    }
                }
            }
            return bestMove;
        }
        bool makeMove(int x,int y){
            if(gameOver()){
                return 0;
            }
            if(!player_board1[(y-1)*5+x-1])
            {
                 player_board1[(y-1)*5+(x-1)]=1;
                 //as soon as the player makes a move the AI also immediately makes a move
                 Move tempMove=findBestMove();
                 player_board1[(tempMove.col*5)+tempMove.row]=2;
                 return 1;
            }
            leftFree-=2;
            return 0;
        }

};
int main(){
    TicTacToe myTTT=TicTacToe();
    int x,y;
    while(!myTTT.gameOver()){
        std::cout<<"-------------------------\n";
        myTTT.printBoard();
        do{
            std::cout<<"Make your move Player X";
            std::cout<<"\nx:";
            std::cin>>x;
            std::cout<<"y:";
            std::cin>>y;
        }while(!myTTT.makeMove(x,y));
    }
}
