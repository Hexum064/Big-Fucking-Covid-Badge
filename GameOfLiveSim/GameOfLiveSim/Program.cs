using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace GameOfLiveSim
{
    class Program
    {
        static byte[] board_0 =
        {
            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b01000000, 0b00000000, 0b00000000, 0b00000000,
            0b01010000, 0b00000000, 0b00000000, 0b00000000,
            0b01100000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,

            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,

            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,

            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000,
            0b00000000, 0b00000000, 0b00000000, 0b00000000

        };

        static byte[] board_1 = new byte[128];

        static void Main(string[] args)
        {
            byte[] sourceBoard = board_0;
            byte[] targetBoard = board_1;

            

            while (true)
            {
                draw_board(sourceBoard);

                Thread.Sleep(500);

                run_game(sourceBoard, targetBoard);

                if ( sourceBoard == board_0)
                {
                    sourceBoard = board_1;
                    targetBoard = board_0;
                }
                else
                {
                    sourceBoard = board_0;
                    targetBoard = board_1;
                }

            }


        }

        static byte run_game(byte[] sourceBoard, byte[] targetBoard)
        {
            ushort b;
            ushort line;
            byte neighbors = 0;
            byte deadBoard = 1;

            for (ushort i = 0; i < 1024; i++)
            {
                neighbors = 0;

                //Checking Top
                if (i > 31) //can only check top if current i is not top
                {
                    line = (ushort)(i - 32); //next line up
                    //Checking left
                    if ((i % 32) > 0)
                    {
                        b = (ushort)(line - 1); //bit to the left
                   
                        if ((sourceBoard[b / 8] & (1 << b % 8)) != 0) 
                        {
                            neighbors++;
                        }

                    }

                    b = line; //next line up directly above
    
                    if ((sourceBoard[b / 8] & (1 << b % 8)) != 0)
                    {
                        neighbors++;
                    }

                    //checking right
                    if ((i % 32) < 31)
                    {
                        b = (ushort)(line + 1); //bit to the right

                        if ((sourceBoard[b / 8] & (1 << b % 8)) != 0)
                        {
                            neighbors++;
                        }
                    }
                }

                //check middle

                line = i; //same line

                //checking left
                if ((i % 32) > 0)
                {
                    b = (ushort)(line - 1); //bit to the left

                    if ((sourceBoard[b / 8] & (1 << b % 8)) != 0)
                    {
                        neighbors++;
                    }

                }

                //checking right
                if ((i % 32) < 31)
                {
                    b = (ushort)(line + 1); //bit to the right

                    if ((sourceBoard[b / 8] & (1 << b % 8)) != 0)
                    {
                        neighbors++;
                    }
                }


                //check bottom
                if (i < 1024 - 32) //can only check bottom if current i is not bottom
                {
                    line = (ushort)(i + 32); //next line down
                    //Checking left
                    if ((i % 32) > 0)
                    {
                        b = (ushort)(line - 1); //bit to the left

                        if ((sourceBoard[b / 8] & (1 << b % 8)) != 0)
                        {
                            neighbors++;
                        }

                    }

                    b = line; //next line up directly below

                    if ((sourceBoard[b / 8] & (1 << b % 8)) != 0)
                    {
                        neighbors++;
                    }

                    //checking right
                    if ((i % 32) < 31)
                    {
                        b = (ushort)(line + 1); //bit to the right

                        if ((sourceBoard[b / 8] & (1 << b % 8)) != 0)
                        {
                            neighbors++;
                        }
                    }
                }

                if (neighbors == 3) //force cell to live
                {
                    targetBoard[i / 8] |= (byte)(1 << i % 8);
                    //We are forcing a cell to live so the board is not dead
                    deadBoard = 0;
                }
                else if (neighbors < 2 || neighbors > 3)
                {
                    //force kill a cell
                    targetBoard[i / 8] &= (byte)~(1 << i % 8);
                }
                else
                {
                    //neighbors == 3 so just pass along the current cell and do a board check
                    byte temp = (byte)(sourceBoard[i / 8] & (byte)(1 << i % 8));

                    //need to force an update on the target board to match the cell from the source board
                    if (temp != 0)
                    {
                        targetBoard[i / 8] |= (byte)(1 << i % 8);
                        deadBoard = 0;
                    }
                    else
                    {
                        targetBoard[i / 8] &= (byte)~(1 << i % 8);
                    }
                }
                
            }

            return deadBoard;
        }

        static void draw_board(byte[] board)
        {
            Console.CursorLeft = 0;
            Console.CursorTop = 0;

            for (byte y = 0; y < 128; y+=4)
            {
                for (byte x = 0; x < 32; x++)
                {
                    int b = y + x / 8; //should be integer math and give y + 0 to 3

                    if ((board[b] & (1 << (x % 8))) != 0) //x % 8 should give the bit pos
                    {
                        Console.Write("*");
                    }
                    else
                    {
                        Console.Write(".");
                    }

                    
                }

                Console.WriteLine();
            }
        }
    }
}
