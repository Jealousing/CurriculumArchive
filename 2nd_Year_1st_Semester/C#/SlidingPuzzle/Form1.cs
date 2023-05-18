using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.VisualStyles;

//제작기간 2020.06.17~2020.06.20
//C#의 Winform을 이용한 슬라이딩 퍼즐 만들기
//제작자 : 서동주

namespace sliding_puzzle
{ 
    public partial class Form1 : Form
    {
        PictureBox puzzle = null;

        protected int[,] m_puzzledata = new int[,] //빈퍼즐판 
        {//==20 벽
             {20,20, 20,20,20,20 }
            ,{20, 0 , 0 , 0 , 0 ,20 } 
            ,{20, 0 , 0 , 0 , 0 ,20 }
            ,{20, 0 , 0 , 0 , 0 ,20 }
            ,{20, 0 , 0 , 0 , 0 ,20 }
            ,{20,20,20,20,20,20 }
        };
        protected int[,] m_answerdata = new int[,]//비교를위한 정답판.
        {
            {20,20, 20,20,20,20 }
            ,{20, 1 , 2  , 3 , 4 ,20 }
            ,{20, 5 , 6 , 7 , 8 ,20 }
            ,{20, 9 , 10 ,11 ,12,20 }
            ,{20, 13 ,14, 15,16,20 }
            ,{20,20,20,20,20,20 }
        };

        PictureBox[,] m_puzzlearray = null;

        //undo를 위한 이동좌표 기억.
        //Stack을 활용한 마지막에 받은 데이터를 제일먼저 쓰도록 사용
        Stack<int> PosxData = new Stack<int>();
        Stack<int> PosyData = new Stack<int>();

        //이미지 크기 배율
        public static int imageratio = 3;
        //이미지 크기
        public int imagesizex = 48 * imageratio;
        public int imagesizey = 32 * imageratio;

        //움직인 횟수
        private int movecount = 0;
        private string movestr = "\r\n"+"  움직인 횟수 : ";

        //시간관련
        DateTime timer = DateTime.Now;
        DateTime timenow = DateTime.Now;
        private string timestr = "\r\n" + "  경과 시간 : ";

        void Init()//초기화 및 게임진행
        {
            WinFormSetting();
            DataMix();
            ImageChange();
            GameClear();
        }

        void WinFormSetting() // Winform 세팅
        {
            int sizex = m_puzzledata.GetLength(1);
            int sizey = m_puzzledata.GetLength(0);
            m_puzzlearray = new PictureBox[sizey, sizex];
            Size formsize = this.Size;
            formsize.Width = (sizex * imagesizex) + 364;
            formsize.Height = (sizey * imagesizey) + 32;
            this.Size = formsize;
            this.BackColor = Color.DarkGray;
            label1.Text = movestr+movecount.ToString();
            TimeSpan timeset= timenow - timer;
            label2.Text = timestr + (timeset.Minutes).ToString("00:") + (timeset.Seconds).ToString("00:") + (timeset.Milliseconds).ToString("00");
        }

        //슬라이딩 퍼즐은 4x4 기준 빈블럭을 제외한 15블럭이 움직인 횟수가 짝수가 되어야되고
        //대각선으로 바뀐건 이 횟수에 포함이 되지않는다.
        //이 규칙이 적용 되지 않으면 50%의 확률로 퍼즐이 완성이 되지않는다.
        //시도한 랜덤배치
        //1. Random을 이용한 좌표마다 다른 블럭 배치
        //2. Random을 이용한 정답에서 블럭들의 위치를 상하좌우중 랜덤적으로  스왑하는 방식.
        //3. Random을 이용해 빈 블럭의 상하좌우중 랜덤으로 선택해 위치를 바꿔주는형식
        private void DataMix() //퍼즐 블럭을 섞어준다.
        {
            //빈블럭을 움직이는 방식

            // n번 (짝수로) 섞기
            // 정답 형태의 상태로 짝수번을 섞는다
            int mixcount = 400;
            //빈 퍼즐에 정답을 복사해준다.
            m_puzzledata = (int[,])m_answerdata.Clone();
            //방향
            Random rndnum = new Random();
            //처음 빈공간의 좌표
            int x = 4;
            int y = 4;
            //데이터를 교환할때 임시저장할 곳
            int tempdata;

            for (int i = 0; i < mixcount;)//짝수n번 반복
            {
                //움직일 방향을 선택한다.
                int choice = rndnum.Next() % 4;
                switch (choice)
                {
                    case 0://상
                        if(m_puzzledata[y-1,x]!=20)//움직일 방향에 벽이 없다면 진행
                        {
                            tempdata = m_puzzledata[y-1 , x];
                            m_puzzledata[y-1 , x] = m_puzzledata[y, x];
                            m_puzzledata[y, x] = tempdata;
                            y--;//빈블럭의 좌표를 저장
                            i++;//반복
                        }
                        break;
                    case 1://하
                        if (m_puzzledata[y+1, x] != 20)//움직일 방향에 벽이 없다면 진행
                        {
                            tempdata = m_puzzledata[y+1, x];
                            m_puzzledata[y+1, x] = m_puzzledata[y, x];
                            m_puzzledata[y, x] = tempdata;
                            y++;//빈블럭의 좌표를 저장
                            i++;//반복
                        }
                        break;
                    case 2://좌
                        if (m_puzzledata[y, x-1] != 20)//움직일 방향에 벽이 없다면 진행
                        {
                            tempdata = m_puzzledata[y, x-1];
                            m_puzzledata[y, x-1] = m_puzzledata[y, x];
                            m_puzzledata[y, x] = tempdata;
                            x--;//빈블럭의 좌표를 저장
                            i++;//반복
                        }
                        break;
                    case 3://우
                        if (m_puzzledata[y, x+1] != 20)//움직일 방향에 벽이 없다면 진행
                        {
                            tempdata = m_puzzledata[y, x+1];
                            m_puzzledata[y, x+1] = m_puzzledata[y, x];
                            m_puzzledata[y, x] = tempdata;
                            x++;//빈블럭의 좌표를 저장
                            i++;//반복
                        }
                        break;
                    default:
                        MessageBox.Show("오류");
                        break;

                }
            }
        }
       

        private void Undo(object sender)//되돌리기
        {
            if(PosxData.Count()!=0&&PosyData.Count()!=0)//Stack에 데이터가 없으면 실행x
            {
                //최근 저장된 두개의 블럭데이터 불러오기
                int x1 = PosxData.Pop();
                int y1 = PosyData.Pop();
                int x2 = PosxData.Pop();
                int y2 = PosyData.Pop();
                //스왑
                int tempdata = m_puzzledata[y1, x1];
                m_puzzledata[y1, x1] = m_puzzledata[y2, x2];
                m_puzzledata[y2, x2] = tempdata;
                //출력
                ImageChange(sender, y1, x1);
                ImageChange(sender, y2, x2);
                //시간밑 움직인횟수 출력
                label1.Text = movestr + (++movecount).ToString();//undo도 결국 움직인것
                timenow = DateTime.Now;//지금시간
                TimeSpan timeset = timenow - timer;//시작시간 -지금시간  = 진행시간
                label2.Text = timestr + (timeset.Minutes).ToString("00:") + (timeset.Seconds).ToString("00:") + (timeset.Milliseconds).ToString("00.");
            }
        }
        public Form1()
        {
            InitializeComponent();
            Init();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
        }

        private void GameClear() // 클리어 되었는지 확인
        {
            int clearcount = 0;//일치된 블럭 갯수확인
            for(int y=0;y<m_puzzledata.GetLength(0);y++)
            {
                for(int x=0; x<m_puzzledata.GetLength(1);x++)
                {
                    if (m_puzzledata[y, x] == m_answerdata[y, x])//현제 플레이중인 블럭판과 정답판을 비교 같은좌표에 같은블럭이있으면 
                        clearcount++;//일치된블럭갯수증가

                    if (clearcount >= m_puzzledata.GetLength(0) * m_puzzledata.GetLength(1))//클리어가 된 경우
                    {
                        //클리어 시간과 움직인 횟수 출력
                        timenow = DateTime.Now;
                        TimeSpan timeset = timenow - timer;
                        label2.Text = timestr + (timeset.Minutes).ToString("00:") + (timeset.Seconds).ToString("00:") + (timeset.Milliseconds).ToString("00.");
                        string showstr = "게임클리어 \r\n" + timestr + (timeset.Minutes).ToString("00:") + (timeset.Seconds).ToString("00:") + (timeset.Milliseconds).ToString("00.") + "\r\n" + movestr + movecount.ToString();
                        MessageBox.Show(showstr);
                        clearcount = 0;
                    }
                        
                }
            }

        }

        private void ImageChange() //저장된 블럭에 따른 이미지 교체
        {
            int sizex = m_puzzledata.GetLength(1);
            int sizey = m_puzzledata.GetLength(0);
            //퍼즐출력
            for (int y = 0; y < sizey; y++)
            {
                for (int x = 0; x < sizex; x++)
                {
                    puzzle = new PictureBox();

                    if (m_puzzledata[y,x] == 16)//검은색 칸
                    {
                        puzzle.Image = Properties.Resources._17;
                    }
                    if (m_puzzledata[y, x] == 1)//n번 블럭
                    {
                        puzzle.Image = Properties.Resources._1;
                    }
                    if (m_puzzledata[y,x]  == 2)
                    {
                        puzzle.Image = Properties.Resources._2;
                    }
                    if (m_puzzledata[y, x] == 3)
                    {
                        puzzle.Image = Properties.Resources._3;
                    }
                    if (m_puzzledata[y, x] == 4)
                    {
                        puzzle.Image = Properties.Resources._4;
                    }
                    if (m_puzzledata[y, x] == 5)
                    {
                        puzzle.Image = Properties.Resources._5;
                    }
                    if (m_puzzledata[y, x] == 6)
                    {
                        puzzle.Image = Properties.Resources._6;
                    }
                    if (m_puzzledata[y, x] == 7)
                    {
                        puzzle.Image = Properties.Resources._7;
                    }
                    if (m_puzzledata[y, x] == 8)
                    {
                        puzzle.Image = Properties.Resources._8;
                    }
                    if (m_puzzledata[y, x] == 9)
                    {
                        puzzle.Image = Properties.Resources._9;
                    }
                    if (m_puzzledata[y, x] == 10)
                    {
                        puzzle.Image = Properties.Resources._10;
                    }
                    if (m_puzzledata[y, x] == 11)
                    {
                        puzzle.Image = Properties.Resources._11;
                    }
                    if (m_puzzledata[y, x] == 12)
                    {
                        puzzle.Image = Properties.Resources._12;
                    }
                    if (m_puzzledata[y, x] == 13)
                    {
                        puzzle.Image = Properties.Resources._13;
                    }
                    if (m_puzzledata[y, x] == 14)
                    {
                        puzzle.Image = Properties.Resources._14;
                    }
                    if (m_puzzledata[y, x] == 15)
                    {
                        puzzle.Image = Properties.Resources._15;
                    }
                    //늘리기모드
                    puzzle.SizeMode = PictureBoxSizeMode.StretchImage;
                    //퍼즐의 사이즈
                    puzzle.Size = new Size(imagesizex, imagesizey);
                    //퍼즐의 위치
                    puzzle.Location = new Point(x * imagesizex, y * imagesizey);

                    m_puzzlearray[y, x] = puzzle;

                    this.Controls.Add(puzzle);
                }
            }
        }
        private void ImageChange(object sender,int y,int x) //좌표를받아 이미지 출력
        {
            PictureBox tempbox = sender as PictureBox;
            tempbox = m_puzzlearray[y, x];
            if (m_puzzledata[y, x] == 16)//검은색 칸
            {
                tempbox.Image = Properties.Resources._17;
            }
            if (m_puzzledata[y, x] == 1)
            {
                tempbox.Image = Properties.Resources._1;
            }
            if (m_puzzledata[y, x] == 2)
            {
                tempbox.Image = Properties.Resources._2;
            }
            if (m_puzzledata[y, x] == 3)
            {
                tempbox.Image = Properties.Resources._3;
            }
            if (m_puzzledata[y, x] == 4)
            {
                tempbox.Image = Properties.Resources._4;
            }
            if (m_puzzledata[y, x] == 5)
            {
                tempbox.Image = Properties.Resources._5;
            }
            if (m_puzzledata[y, x] == 6)
            {
                tempbox.Image = Properties.Resources._6;
            }
            if (m_puzzledata[y, x] == 7)
            {
                tempbox.Image = Properties.Resources._7;
            }
            if (m_puzzledata[y, x] == 8)
            {
                tempbox.Image = Properties.Resources._8;
            }
            if (m_puzzledata[y, x] == 9)
            {
                tempbox.Image = Properties.Resources._9;
            }
            if (m_puzzledata[y, x] == 10)
            {
                tempbox.Image = Properties.Resources._10;
            }
            if (m_puzzledata[y, x] == 11)
            {
                tempbox.Image = Properties.Resources._11;
            }
            if (m_puzzledata[y, x] == 12)
            {
                tempbox.Image = Properties.Resources._12;
            }
            if (m_puzzledata[y, x] == 13)
            {
                tempbox.Image = Properties.Resources._13;
            }
            if (m_puzzledata[y, x] == 14)
            {
                tempbox.Image = Properties.Resources._14;
            }
            if (m_puzzledata[y, x] == 15)
            {
                tempbox.Image = Properties.Resources._15;
            }
        }

        private void Image_Up(object sender)//위로 움직임
        {
            PictureBox tempbox = sender as PictureBox;
            //판사이즈
            int sizex = m_puzzledata.GetLength(1);
            int sizey = m_puzzledata.GetLength(0);
            //좌표
            int posx = 0;
            int posy = 0;
            //누르면 빈공간을 찾는다
            for (int y = 0; y < sizey; y++)
            {
                for (int x = 0; x < sizex; x++)
                {
                    if (m_puzzledata[y, x] == 16)//빈블럭찾기
                    {
                        posx = x; posy = y;//빈블럭의 좌표 불러오기
                    }
                }
            }

            //빈공간으로 움직으로 움직일수있으면
            if (posy != 0 && m_puzzledata[posy - 1, posx] != 20)
            {
                //빈데이터에 빈블럭을 저장하고
                int num = m_puzzledata[posy, posx];
                //움직일 방향의 블럭을 빈블럭이 있던곳에 저장
                m_puzzledata[posy, posx] = m_puzzledata[posy - 1, posx];
                //이미지 출력밑 데이터 교체
                tempbox = m_puzzlearray[posy, posx];
                ImageChange(sender, posy, posx);
                m_puzzledata[posy - 1, posx] = num;
                tempbox = m_puzzlearray[posy - 1, posx];
                tempbox.Image = Properties.Resources._17;
                //움직인 활동저장
                PosxData.Push(posx);
                PosyData.Push(posy);
                PosxData.Push(posx);
                PosyData.Push(posy-1);
                //정보출력
                label1.Text = movestr + (++movecount).ToString();
                timenow = DateTime.Now;
                TimeSpan timeset = timenow - timer;
                label2.Text = timestr + (timeset.Minutes).ToString("00:") + (timeset.Seconds).ToString("00:") + (timeset.Milliseconds).ToString("00");
            }
            else
            {
            }
        }
        private void Image_Down(object sender)//이미지 밑으로 움직이기  Image_Up과 동일
        {
            PictureBox tempbox = sender as PictureBox;
            int sizex = m_puzzledata.GetLength(1);
            int sizey = m_puzzledata.GetLength(0);
            int posx = 0;
            int posy = 0;
            //누르면 빈공간을 찾는다
            for (int y = 0; y < sizey; y++)
            {
                for (int x = 0; x < sizex; x++)
                {
                    if (m_puzzledata[y, x] == 16)
                    {
                        posx = x; posy = y;
                    }
                }
            }


            //빈공간으로 움직으로 움직일수있으면
            if (m_puzzledata[posy + 1, posx] != 20)
            {
                int num = m_puzzledata[posy, posx];

                m_puzzledata[posy, posx] = m_puzzledata[posy + 1, posx];
                tempbox = m_puzzlearray[posy, posx];
                ImageChange(sender, posy, posx);


                m_puzzledata[posy + 1, posx] = num;
                tempbox = m_puzzlearray[posy + 1, posx];
                tempbox.Image = Properties.Resources._17;
                PosxData.Push(posx);
                PosyData.Push(posy);
                PosxData.Push(posx);
                PosyData.Push(posy+1);
                label1.Text = movestr + (++movecount).ToString();
                timenow = DateTime.Now;
                TimeSpan timeset = timenow - timer;
                label2.Text = timestr + (timeset.Minutes).ToString("00:") + (timeset.Seconds).ToString("00:") + (timeset.Milliseconds).ToString("00");
            }
            else
            {
            }
        }
        private void Image_Left(object sender)//이미지 옆으로 움직이기  Image_Up과 동일
        {
            PictureBox tempbox = sender as PictureBox;
            int sizex = m_puzzledata.GetLength(1);
            int sizey = m_puzzledata.GetLength(0);
            int posx = 0;
            int posy = 0;
            //누르면 빈공간을 찾는다
            for (int y = 0; y < sizey; y++)
            {
                for (int x = 0; x < sizex; x++)
                {
                    if (m_puzzledata[y, x] == 16)
                    {
                        posx = x; posy = y;
                    }
                }
            }


            //빈공간으로 움직으로 움직일수있으면
            if (m_puzzledata[posy, posx - 1] != 20)
            {
                int num = m_puzzledata[posy, posx];

                m_puzzledata[posy, posx] = m_puzzledata[posy, posx - 1];
                tempbox = m_puzzlearray[posy, posx];
                ImageChange(sender, posy, posx);


                m_puzzledata[posy, posx - 1] = num;
                tempbox = m_puzzlearray[posy, posx - 1];
                tempbox.Image = Properties.Resources._17;
                PosxData.Push(posx);
                PosyData.Push(posy);
                PosxData.Push(posx-1);
                PosyData.Push(posy);
                label1.Text = movestr + (++movecount).ToString();
                timenow = DateTime.Now;
                TimeSpan timeset = timenow - timer;
                label2.Text = timestr + (timeset.Minutes).ToString("00:") + (timeset.Seconds).ToString("00:") + (timeset.Milliseconds).ToString("00");
            }
            else
            {
            }
        }
        private void Image_Right(object sender)//이미지 옆으로 움직이기  Image_Up과 동일
        {
            PictureBox tempbox = sender as PictureBox;
            int sizex = m_puzzledata.GetLength(1);
            int sizey = m_puzzledata.GetLength(0);
            int posx = 0;
            int posy = 0;
            //누르면 빈공간을 찾는다
            for (int y = 0; y < sizey; y++)
            {
                for (int x = 0; x < sizex; x++)
                {
                    if (m_puzzledata[y, x] == 16)
                    {
                        posx = x; posy = y;
                    }
                }
            }


            //빈공간으로 움직으로 움직일수있으면
            if (m_puzzledata[posy, posx + 1] != 20)
            {
                int num = m_puzzledata[posy, posx];

                m_puzzledata[posy, posx] = m_puzzledata[posy, posx + 1];
                tempbox = m_puzzlearray[posy, posx];
                ImageChange(sender, posy, posx);


                m_puzzledata[posy, posx + 1] = num;
                tempbox = m_puzzlearray[posy, posx + 1];
                tempbox.Image = Properties.Resources._17;
                PosxData.Push(posx);
                PosyData.Push(posy);
                PosxData.Push(posx+1);
                PosyData.Push(posy);
                label1.Text = movestr + (++movecount).ToString();
                timenow = DateTime.Now;
                TimeSpan timeset = timenow - timer;
                label2.Text = timestr + (timeset.Minutes).ToString("00:")+ (timeset.Seconds).ToString("00:") + (timeset.Milliseconds).ToString("00");
            }
            else
            {
            }
        }

        private void pictureBox2_MouseClick(object sender, MouseEventArgs e)//방향버튼 위 클릭
        {
            GameClear();
            if (e.Button == MouseButtons.Left)
            {
                Image_Up(sender);
            }
            GameClear();
        }
        private void pictureBox3_MouseClick(object sender, MouseEventArgs e)//방향버튼 왼쪽 클릭
        {
            GameClear();
            if (e.Button == MouseButtons.Left)
            {
                Image_Left(sender);
            }
            GameClear();
        }
        private void pictureBox4_MouseClick(object sender, MouseEventArgs e)//방향버튼 밑 클릭
        {
            GameClear();
            if (e.Button == MouseButtons.Left)
            {
                Image_Down(sender);
            }
            GameClear();
        }
        private void pictureBox5_MouseClick(object sender, MouseEventArgs e)//방향버튼 오른쪽클릭
        {
            GameClear();
            if (e.Button == MouseButtons.Left)
            {
                Image_Right(sender);
            }
            GameClear();
        }
        private void pictureBox7_MouseClick(object sender, MouseEventArgs e)//undo 버튼클릭
        {
            if (e.Button == MouseButtons.Left)
            {
                Undo(sender); 
            }
        }
        private void pictureBox1_MouseClick(object sender, MouseEventArgs e) //정답이미지 클릭 반응x
        {
            if (e.Button == MouseButtons.Left)
            {
            }
        }
        private void Form1_KeyDown(object sender, KeyEventArgs e) //키보드 클릭
        {
            switch(e.KeyCode)
            {//키보드 wasd와 방향키 인식
                case Keys.Up:
                case Keys.W:
                    Image_Up(sender);
                    GameClear();
                    break;
                case Keys.Down:
                case Keys.S:
                    Image_Down(sender);
                    GameClear();
                    break;
                case Keys.Left:
                case Keys.A:
                    Image_Left(sender);
                    GameClear();
                    break;
                case Keys.Right:
                case Keys.D:
                    Image_Right(sender);
                    GameClear();
                    break;
                case Keys.Z:
                    Undo(sender);
                    break;
            }
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void label2_Click(object sender, EventArgs e)
        {

        }
    }
}
