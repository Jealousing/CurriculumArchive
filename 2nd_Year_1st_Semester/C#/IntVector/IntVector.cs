using System;
using System.Collections;
using System.Collections.Generic;

namespace SeoDongJu
{
    //2020.05.02 
    public class IntVector
    {
        int[] m_ArrayInt = new int[2];
        int m_Length = 0;
        private int count;
        public int Count
        {
            get { return count; }
            set { count = value; }
        }
        public string GetString()//출력
        {
            if (m_Length <= 0)
                return "null";

            string outstr = "";
            for (int i = 0; i < m_Length; i++)
            {
                outstr += string.Format($"{m_ArrayInt[i]}");
                if(i != m_Length-1)
                {
                    outstr += string.Format(", ");
                }
            }
            return outstr;
        }
        public void Push(int p_val) //입력
        {
            if (m_Length >= m_ArrayInt.Length)
            {
                int tempsize = m_Length * 2;
                int[] tempint = new int[tempsize];

                for (int i = 0; i < m_ArrayInt.Length; i++)
                {
                    tempint[i] = m_ArrayInt[i];
                }
                m_ArrayInt = tempint;
            }
            m_ArrayInt[m_Length++] = p_val;
            Count = m_Length;
        }

        public void RemoveAt(int p_index) //부분 삭제
        {
            for (int i = p_index; i < m_Length - 1; i++)
            {
                m_ArrayInt[i] = m_ArrayInt[i + 1];
            }
            m_Length--;
            Count = m_Length;
        }
        public void Clear()//전부삭제
        {
            m_ArrayInt = null;
            m_ArrayInt = new int[2];
            m_Length = 0;
            Count = m_Length;
        }

        public void InsertAt(int p_index, int p_val) //index 3 하면 3번째자리에 p_val값을 넣고 밀어내기
        {
            if (m_Length >= m_ArrayInt.Length)
            {
                int tempsize = m_Length * 2;
                int[] tempint = new int[tempsize];

                for (int i = 0; i < m_ArrayInt.Length; i++)
                {
                    tempint[i] = m_ArrayInt[i];
                }
                m_ArrayInt = tempint;
            }
            for (int i = m_Length; i > p_index; i--)
            {
                m_ArrayInt[i] = m_ArrayInt[i - 1]; 
            }
            m_Length++;
            Count = m_Length;
            m_ArrayInt[p_index] = p_val;
            
        }

        public void Addrange(int p_index, int[] p_val)//index 3하면 3번째자리에 배열의 갯수만큼 다넣고 밀어내기
        {
            if (m_Length+p_val.Length >= m_ArrayInt.Length)
            {
                int tempsize = (m_Length + p_val.Length)*2;
                int[] tempint = new int[tempsize];

                for (int i = 0; i < m_ArrayInt.Length; i++)
                {
                    tempint[i] = m_ArrayInt[i];
                }
                m_ArrayInt = tempint;
            }
            int[] inttemp = new int[p_val.Length];
           for(int i=0;i<m_ArrayInt.Length;i++)
            {
                if(m_ArrayInt[p_index+i]!=0)
                {
                    inttemp[i] = m_ArrayInt[p_index + i];
                }
                else
                {
                    break;
                }
            }
            m_Length += p_val.Length;
            Count = m_Length;
            for (int j = 0; j < p_val.Length; j++)
            {
                m_ArrayInt[p_index + j] = p_val[j];
            }

            for (int o = 0; o< m_Length-p_val.Length ; o++)
            {
                int u = 0;
                for(int p=0;p<m_ArrayInt.Length;p++)
                {
                    if(m_ArrayInt[p]==0)
                    {
                        u++;
                        m_ArrayInt[p] = inttemp[o];
                    }
                    if(u!=0)
                    {
                        break;
                    }
                }
            }
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            IntVector tempvector = new IntVector();
            int[] num = { 1 ,2,3};

            Console.WriteLine("-----------------------------");
            tempvector.Push(10);//숫자입력
            tempvector.Push(20);
            tempvector.Push(30);
            Console.WriteLine("저장된 숫자: "+tempvector.GetString());
            Console.WriteLine("저장된 숫자의 개수: " + tempvector.Count+"\t\n");


            tempvector.RemoveAt(1); //1번칸 삭제
            tempvector.InsertAt(1, 15);//1번칸 15입력
            Console.WriteLine("저장된 숫자: " + tempvector.GetString());
            Console.WriteLine("저장된 숫자의 개수: " + tempvector.Count + "\t\n");

            tempvector.Clear();//모두삭제
            Console.WriteLine("저장된 숫자: " + tempvector.GetString());
            Console.WriteLine("저장된 숫자의 개수: " + tempvector.Count + "\t\n");

            tempvector.Push(20);//숫자입력
            tempvector.Push(30);
            Console.WriteLine("저장된 숫자: " + tempvector.GetString());
            Console.WriteLine("저장된 숫자의 개수: " + tempvector.Count + "\t\n");

            tempvector.Addrange(1, num);//숫자배열 1번칸에 입력
            Console.WriteLine("저장된 숫자: " + tempvector.GetString());
            Console.WriteLine("저장된 숫자의 개수: " + tempvector.Count + "\t\n");
            Console.WriteLine("-----------------------------");
        }
    }
}
