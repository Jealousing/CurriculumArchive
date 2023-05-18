using DongJuPlantsVsZombies.Properties;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml;

/*
    2020-07-16
 */

namespace DongJuPlantsVsZombies
{
    public partial class Form1 : Form
    {
        int[,] StageData =
        {
            {0,0,0,0,0,0,0,0,0 },
            {0,0,0,0,0,0,0,0,0 },
            {0,0,0,0,0,0,0,0,0 },
            {0,0,0,0,0,0,0,0,0 },
            {0,0,0,0,0,0,0,0,0 }
        };//식물과 좀비 판 사이즈 가로9 세로5

        int[,] CurrentData = null; //현재 데이터

        //라운드 진행 관련 변수
        const int RoundMax = 10000;//라운드 진행 최댓값
        int Round = 0;//현재 라운드
        int ZombieNum = 0;//생성해야되는 좀비수

        //게임 재화 및 점수
        int SunCoin = 100;//돈
        int Score = 0;//점수

        //마우스 좌표 저장
        int MouseX;
        int MouseY;

        //스택으로 쓸것들
        int SunStack = 0;//해 속도 조정
        int BulletStack = 0;//총알 속도 조정
        int TimeStack1 = 0;//시간조정
        int TimeStack2 = 0;//2

        //선택
        int SelectType = 0;
        bool select = false;

        //이미지 사이즈 설정(판)
        public static readonly Size ImageSize = new Size(50, 50);

        //PictureBox 설정과 List들 설정
        PictureBox[,] PictureBoxArr = null;
        //총알의 현재위치와 저장된 위치 리스트
        List<PictureBox> PlantBulletList = new List<PictureBox>();
        List<PictureBox> PlantBulletSaveList = new List<PictureBox>();
        //생상되는 해의 현재 위치와 시작위치 리스트
        List<PictureBox> PlantSunList = new List<PictureBox>();
        List<PictureBox> PlantSunSaveList = new List<PictureBox>();
        //좀비의 현재위치와 저장위치
        List<PictureBox> ZombieList = new List<PictureBox>();
        List<PictureBox> ZombieSaveList = new List<PictureBox>();
        //라운드 저장용
        List<int> RoundList = new List<int>();

        //랜덤사용
        Random random = new Random();

        //실행 관련된 함수
        public Form1()//Winform
        {
            InitializeComponent();
            InitPictureBox();//초기화 설정
            CreateRound();//라운드 생성
            Render();//랜더링~
            TimerSet();//타이머 세팅
        }
        void CreateRound()//라운드 생성 함수
        {
            for (int i = 0; i < RoundMax; i++)
            {
                if (i < 10)//10라운드전 
                    RoundList.Add(i);
                else//이후
                    RoundList.Add(i+2*i);
            }
        }
        void TimerSet()//타이머생성
        {
            Timer timer = new Timer();
            timer.Interval = 50;
            timer.Tick += new EventHandler(Timer_Tick);
            timer.Start();
        }
        void InitPictureBox() //초기설정
        {
            RoundList.Add(0);//첫라운드는 0마리
            ZombieNum = RoundList[0];//좀비숫자 설정 0마리
            int sizeX = StageData.GetLength(1);//배열의 x크기 
            int sizeY = StageData.GetLength(0);//배열의 y크기
            textBox1.Text = SunCoin.ToString();//초기코인 표시
            if (PictureBoxArr != null)
            {
                for (int y = 0; y < PictureBoxArr.GetLength(0); y++)
                {
                    for (int x = 0; x < PictureBoxArr.GetLength(1); x++)
                    {
                        PictureBoxArr[y, x].Dispose();
                    }
                }
                PictureBoxArr = null;
            }
            CurrentData = new int[sizeY, sizeX];

            PictureBoxArr = new PictureBox[sizeY, sizeX];

            for (int y = 0; y < sizeY; y++)//배경생성
            {
                for (int x = 0; x < sizeX; x++)
                {
                    PictureBox BG = new PictureBox();//PictureBox설정
                    BG.Location = new Point(x * ImageSize.Width, y * ImageSize.Height);
                    BG.Size = new Size(ImageSize.Width, ImageSize.Height);
                    BG.SizeMode = PictureBoxSizeMode.StretchImage;
                    this.Controls.Add(BG);
                    BG.Enabled = false;
                    PictureBoxArr[y, x] = BG;

                    CurrentData[y, x] = 0;
                    if (StageData[y, x] == 0 || StageData[y, x] == 1 || StageData[y, x] == 2)//처음 배열저장되있는걸 이동저장
                        CurrentData[y, x] = StageData[y, x];
                }
            }
        }
        void Render()//랜더함수
        {
            PictureBox box = new PictureBox();
            int sizeX = StageData.GetLength(1);
            int sizeY = StageData.GetLength(0);
            for (int y = 0; y < sizeY; y++)//배열에 저장된 정보에따라 맞춰서 출력
            {
                for (int x = 0; x < sizeX; x++)
                {
                    box = PictureBoxArr[y, x];
                    box.Image = null;
                    if (CurrentData[y, x] == 0)
                    {
                        box.Image = Resources.grass;
                    }
                    if (CurrentData[y, x] == 1)
                    {
                        box.Image = Resources.sunflower_01;
                        box.BackgroundImage = Resources.grass;
                        box.BackgroundImageLayout = ImageLayout.Stretch;
                    }
                    if (CurrentData[y, x] == 2)
                    {
                        box.Image = Resources.plant01;
                        box.BackgroundImage = Resources.grass;
                        box.BackgroundImageLayout = ImageLayout.Stretch;
                    }
                }
            }
            if (select)//마우스가 배열안에 들어가면 어디 배열에 마우스가 있는지 표시
            {
                if (MouseX < 9 && MouseY < 5)
                {
                    if (CurrentData[MouseY, MouseX] == 0)
                    {
                        box = PictureBoxArr[MouseY, MouseX];
                        box.Image = Resources.selecticon;
                        box.BackgroundImageLayout = ImageLayout.Stretch;
                        box.BackgroundImage = Resources.grass;
                    }
                }
            }
        }
        void ReZombie()//좀비 생성을 불러오는 함수
        {
            if (ZombieNum <= 0 && Round != RoundMax)
            {
                ZombieNum = RoundList[Round++];//좀비가 0마리면 라운드에 저장되어있는 숫자를 넣어줌
            }
            else
            {
                for (int i = 0; i < ZombieNum; i++)
                {
                    CreateZombieFirst();//좀비 생성!
                }
            }
        }
        void Timer_Tick(object sender, EventArgs e)//일정시간(타이머)마다 실행되는 함수
        {
            ZombieKill();//좀비가 죽었는지확인
            PlantKill();//식물이 죽었는지 확인
            Display();//화면 출력
            if (TimeStack1 == 25)
            {
                ReMake();//식물의 오브젝트 생성
                TimeStack1 = 0;
            }
            if (TimeStack2 == 50)
            {
                ReZombie();//좀비 다시생성
                TimeStack2 = 0;
            }
            TimeStack1++;
            TimeStack2++;
        }
        void ReMake()//식물의 오브젝트 다시 생성
        {
            BulletStack++;
            if (BulletStack == 3)
            {
                for (int i = 0; i < PlantBulletSaveList.Count; i++)
                {
                    PictureBox item = PlantBulletSaveList[i];//현재 남아있는 총알 식물의 오브젝트 데이터 넘겨줌
                    Point pos = item.Location;
                    if (CheckZombie(item))
                        CreateBullet(pos);//총알 발사
                }
                BulletStack = 0;
            }

            SunStack++;
            if (SunStack == 2)
            {
                for (int i = 0; i < PlantSunSaveList.Count; i++)//현재 남아있는 해바라기의 오브젝트 데이터 넘겨줌
                {
                    PictureBox item = PlantSunSaveList[i];
                    Point pos = item.Location;
                    CreateSun(pos);//해코인 생성
                }
                SunStack = 0;
            }
            for (int i = 0; i < ZombieSaveList.Count; i++)//좀비리스트에 저장된 좀비를 출력함
            {
                PictureBox item = ZombieSaveList[i];
                Point pos = item.Location;
                CreateZombie(pos);//좀비 생성
                ZombieSaveList.RemoveAt(i);//생성된 좀비는 삭제
            }
        }
        void Display()//화면 출력 (배경이외의 오브젝트)
        {
            //움직이는 함수들
            foreach (var item in PlantBulletList)//총알
            {
                Point pos = item.Location;
                pos.X += 5;

                item.Location = pos;
                if (pos.X > 600)//좌표벗어나면 사라짐
                {
                    item.Visible = false;
                }
            }
            for (int i = 0; i < PlantSunList.Count; i++)//해
            {
                PictureBox item = PlantSunList[i];
                Point pos = item.Location;


                pos.Y -= 5;

                item.Location = pos;
                if (pos.Y <= 0)//좌표벗어나면 사라짐
                {
                    item.Visible = false;
                }
            }
            for (int i = 0; i < ZombieList.Count; i++)//좀비
            {
                PictureBox item = ZombieList[i];
                Point pos = item.Location;

                pos.X -= 3;
                item.Location = pos;
                if (pos.X < 0)//좌표벗어나면 게임끝
                {
                    Application.Exit();
                    item.Visible = false;
                }
            }

            //사라진것들의 처리
            for (int i = PlantBulletList.Count - 1; i >= 0; i--) //총알사라짐
            {
                if (PlantBulletList[i].Visible == false)
                {
                    PlantBulletList[i].Dispose();
                    PlantBulletList.RemoveAt(i);
                }
            }
            for (int i = PlantSunList.Count - 1; i >= 0; i--)//해 사라지면 코인을 얻고 코인정보창 재출력
            {
                if (PlantSunList[i].Visible == false)
                {
                    PlantSunList[i].Dispose();
                    PlantSunList.RemoveAt(i);
                    SunCoin += 15;
                    textBox1.Text = SunCoin.ToString();
                }
            }
            for (int i = ZombieList.Count - 1; i >= 0; i--)//좀비가 사라지면 스코어점수와 돈을 올려주며 해당정보 재출력
            {
                if (ZombieList[i].Visible == false)
                {
                    ZombieList[i].Dispose();
                    ZombieList.RemoveAt(i);
                    ZombieNum--;
                    SunCoin += 5;
                    Score += 10;
                    textBox1.Text = SunCoin.ToString();
                    textBox4.Text = Score.ToString();
                }
            }
        }


        //식물과 좀비 생성
        bool CheckZombie(PictureBox item)//좀비 체크
        {
            Point pos = item.Location;
            for (int i = 0; i < ZombieList.Count; i++)
            {
                PictureBox item2 = ZombieList[i];
                Point pos2 = item2.Location;
                if (pos.Y - 10 <= pos2.Y && pos.Y + 20 >= pos2.Y)//좀비가 해당좌표에 있는지 확인
                    return true;
            }
            return false;
        }
        void CreateBulletFirst(Point p_pos)//처음 총알 생성하는 함수
        {
            if (PlantBulletSaveList.Count >= 100)
                return;

            PictureBox Bullet = new PictureBox();
            Bullet.Location = p_pos;
            PlantBulletSaveList.Add(Bullet);//좌표를 저장
        }
        void CreateSunFirst(Point p_pos)//처음 해 생성하는 함수
        {
            if (PlantSunSaveList.Count >= 100)
                return;

            PictureBox Bullet = new PictureBox();
            Bullet.Location = p_pos;
            PlantSunSaveList.Add(Bullet);//좌표를 저장
        }
        void CreateZombieFirst()//처음 좀비를 생성하는 함수
        {
            if (ZombieNum <= 0)
                return;
            int num = random.Next(0, 5);//랜덤한 줄에 생성하기위해 쓰는 함수
            PictureBox Zombie = new PictureBox();
            Point pos = Zombie.Location;//좌표 지정
            pos.X = num * 10 + 700;
            pos.Y = num * 50;
            Zombie.Location = pos;
            ZombieSaveList.Add(Zombie);//좌표 저장
        }
        void CreateBullet(Point p_pos)//처음 이후 총알 생성하는 함수
        {
            PictureBox Bullet = new PictureBox();//생성
            Bullet.BackColor = Color.Transparent;//뒷배경설정(적용안됨;)
            Bullet.Image = Resources.bullet;//총알이미지
            Bullet.Location = p_pos;//좌표설정
            Bullet.Size = new System.Drawing.Size(30, 30);//이미지크기
            Bullet.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;//사이즈모드

            this.Controls.Add(Bullet);
            Bullet.BringToFront();

            PlantBulletList.Add(Bullet);//리스트추가
        }
        void CreateSun(Point p_pos)//처음 이후 해 생성하는 함수
        {//구조는 위와 동일
            PictureBox Sun = new PictureBox();
            Sun.BackColor = Color.Transparent;
            Sun.Image = Resources.sun;
            Sun.Location = p_pos;
            Sun.Size = new System.Drawing.Size(30, 30);
            Sun.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;

            this.Controls.Add(Sun);
            Sun.BringToFront();

            PlantSunList.Add(Sun);
        }
        void CreateZombie(Point p_pos)//좀비 설정
        {//구조는 위와 동일
            PictureBox Zombie = new PictureBox();
            Zombie.BackColor = Color.Transparent;
            Zombie.Image = Resources.zombi01_O;
            Zombie.Location = p_pos;
            Zombie.Size = new Size(30, 50);
            Zombie.SizeMode = PictureBoxSizeMode.StretchImage;

            this.Controls.Add(Zombie);
            Zombie.BringToFront();

            ZombieList.Add(Zombie);
        }
        void CreatePlant(int posx, int posy)//클릭으로 식물을 생성하는 함수
        {
            if (CurrentData[posy, posx] == 0)//비어있는 곳에만 설치가능
            {
                if (SelectType == 1)//선택된 식물 타입 받기
                {
                    if (SunCoin >= 50)//코인확인
                    {
                        SunCoin -= 50;//가격 지불
                        CurrentData[posy, posx] = 1;//데이터 변환
                        CreateSunFirst(new Point(posx * ImageSize.Width + 10, posy * ImageSize.Height - 10));//해당 오브젝트 생성
                    }

                }
                if (SelectType == 2)//선택된 식물 타입 받기
                {
                    if (SunCoin >= 100)//코인확인
                    {
                        SunCoin -= 100;//가격 지불
                        CurrentData[posy, posx] = 2;//데이터 변환
                        CreateBulletFirst(new Point(posx * ImageSize.Width + 40, posy * ImageSize.Height));//해당 오브젝트 생성
                    }

                }
            }
            textBox1.Text = SunCoin.ToString();//코인이 변경되었기때문에 재출력
            Render();//랜더링~
        }
       


        //죽음시 제거
        void ZombieKill()//좀비 죽음처리
        {
            for (int i = 0; i < PlantBulletList.Count; i++)//총알의 정보를 입력받고
            {
                PictureBox item1 = PlantBulletList[i];
                Point pos1 = item1.Location;
                for (int j = 0; j < ZombieList.Count; j++)//좀비의 정보를 입력받고
                {
                    PictureBox item2 = ZombieList[j];
                    Point pos2 = item2.Location;
                    if (pos1.X - 10 <= pos2.X && pos1.X + 5 >= pos2.X)//위치비교후 삭제
                    {
                        if (pos1.Y - 10 <= pos2.Y && pos1.Y + 20 >= pos2.Y)
                        {
                            item1.Visible = false;
                            item2.Visible = false;
                        }
                    }
                }
            }
        }
        void PlantKill()//식물 죽음처리
        {
            for (int i = 0; i < ZombieList.Count; i++)//존재하는 좀비의 수만큼 반복
            {
                PictureBox item = ZombieList[i];
                Point point = item.Location;//좌표
                int x = point.X / ImageSize.Width;//2차원배열에 맞게 변경
                int y = point.Y / ImageSize.Height;
                if (x < 9 && y < 5)//이차원 배열안에 들어갈수 있으면
                {
                    if (CurrentData[y, x] == 1)//좀비의 위치에 해바라기가 있으면
                    {
                        CurrentData[y, x] = 0;//빈곳으로 만들고
                        for (int j = 0; j < PlantSunSaveList.Count; j++)//돈만들던 부분 찾아서 삭제
                        {
                            PictureBox item2 = PlantSunSaveList[j];
                            Point point2 = item2.Location;
                            if (point.X >= point2.X - 50 && point.X <= point2.X + 50)
                            {
                                if (point.Y >= point2.Y - 30 && point.Y <= point2.Y + 30)
                                {
                                    PlantSunSaveList[j].Dispose();
                                    PlantSunSaveList.RemoveAt(j);
                                }
                            }
                        }
                    }
                    if (CurrentData[y, x] == 2)//좀비위치에 총알식물이있으면
                    {
                        CurrentData[y, x] = 0;//빈곳으로 만들고
                        for (int j = 0; j < PlantBulletSaveList.Count; j++)//총알쏘던 부분 찾아서 삭제
                        {
                            PictureBox item2 = PlantBulletSaveList[j];
                            Point point2 = item2.Location;
                            if (point.X >= point2.X - 50 && point.X <= point2.X + 50)
                            {
                                if (point.Y >= point2.Y - 30 && point.Y <= point2.Y + 30)
                                {
                                    PlantBulletSaveList[j].Dispose();
                                    PlantBulletSaveList.RemoveAt(j);
                                }
                            }
                        }
                    }
                    Render();
                }
            }
        }

        //클릭관련 함수
        public static Cursor CreateCursor(Bitmap bm, Size size)//커서변경함수
        {
            bm = new Bitmap(bm, size);//비트맵받아와서
            bm.MakeTransparent();//배경투명화
            return new Cursor(bm.GetHicon());//커서설정
        }
        void ClickData(int num)//클릭에 따른 선택이미지 보여주는 함수
        {
            PictureBox Click = new PictureBox();
            Click.Location = new Point(60, 300);
            Click.Size = new Size(ImageSize.Width, ImageSize.Height);
            Click.SizeMode = PictureBoxSizeMode.StretchImage;
            pictureBox1.BackgroundImage = Resources.grass;
            if (num == 1)// 해바라기 or 총알식물
            {
                pictureBox1.Image = Resources.sunflower_01;
            }
            if (num == 2)
            {
                pictureBox1.Image = Resources.plant01;
            }
        }
        private void Form1_MouseDown(object sender, MouseEventArgs e)//마우스가 눌리면
        {
            if (e.Location.X < ImageSize.Width * 9 && e.Location.Y < ImageSize.Height * 5)
            {//눌린 곳의 좌표가 식물이 설치되는 좌표에 들어가 있으면 실행
                MouseX = e.Location.X / ImageSize.Width; //마우스의 좌표를 이미지 사이즈로 나누어 좌표생성
                MouseY = e.Location.Y / ImageSize.Height;
                CreatePlant(MouseX, MouseY);//식물생성
            }
        }
        private void Form1_MouseMove(object sender, MouseEventArgs e)//마우스가 어디있는지 확인하는 함수
        {
            MouseX = e.Location.X / ImageSize.Width;//마우스 좌표를 알맞게 변경
            MouseY = e.Location.Y / ImageSize.Height;
            if (e.Location.X < ImageSize.Width * 9 && e.Location.Y < ImageSize.Height * 5)
            {
                select = true;//선택정보 저장
                Render();//랜더링
            }
        }
        private void pictureBox1_Click(object sender, EventArgs e)//설치할 식물을 선택하는 함수 (해바라기)
        {
            SelectType = 1;//선택한 식물의 타입을 변경
            ClickData(SelectType);//식물타입을 넘겨줌
            Bitmap bitmap = (Bitmap)Resources.sunflower_01;//커서모양 변경
            this.Cursor = CreateCursor(bitmap, new Size(30, 30));
        }
        private void pictureBox2_Click(object sender, EventArgs e)//바로위의 함수와 동일하며 총알식물이다.
        {
            SelectType = 2;//선택한 식물의 타입을 변경
            ClickData(SelectType);//식물타입을 넘겨줌
            Bitmap bitmap = (Bitmap)Resources.plant01;//커서모양 변경
            this.Cursor = CreateCursor(bitmap, new Size(30, 30));
        }
        private void pictureBox1_Click_1(object sender, EventArgs e)//지정된 식물을 푸는 버튼으로 처음에 비어있는 사진쪽을 누르면 선택해제
        {
            SelectType = 0;
            this.Cursor = Cursors.Default;
        }
        private void Form1_Load(object sender, EventArgs e)
        {
        }
        private void pictureBox3_MouseMove(object sender, MouseEventArgs e)//움직이면 선택된 칸 해제
        {
            select = false;
            Render();
        }
        private void pictureBox2_MouseMove_1(object sender, MouseEventArgs e)//움직이면 선택된 칸 해제
        {
            select = false;
            Render();
        }
        private void Form1_KeyDown(object sender, KeyEventArgs e)//esc누르면 종료
        {
            if(e.KeyCode==Keys.Escape)
            {
                Application.Exit();
            }
        }

    }
}