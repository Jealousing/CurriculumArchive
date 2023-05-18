using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerTargeting : MonoBehaviour
{
    //타겟팅과 공격 설정
    public GameObject Trickle;//발사체

    //타겟이될 몬스터 리스트
    public static List<GameObject> Player_Monster_List = new List<GameObject>();

    //딜레이
    public float DelaySec = 2f;
    float timer1 = 0f;

    //bool
    bool IsTarget = false;
    bool  IsFirst = false;
    public static bool IsAttack = false;

    //발사체 설정
    public static int Bulletnum = 1;
    public static bool IsSideway = false;
    public static bool IsBackway = false;
    public static bool IsDiagonally = false;

    float currentDist = 0;      //현재 거리
    float closetDist = 100f;    //가까운 거리
    float TargetDist = 100f;   //타겟 거리
    int closeDistIndex = 0;    //가장 가까운 인덱스
    public int TargetIndex = -1;      //타겟팅 할 인덱스
    public LayerMask layerMask;

    void Start()
    {
        timer1 = DelaySec;
        Trickle.SetActive(false);
    }

    //타겟팅
    void Targeting()
    {
        if (PlayerMove.Isruning)//움직이고있으면 실행x
        {
            IsFirst = false;
            IsTarget = false;
            return;
        }
        else 
        {
            if (Player_Monster_List.Count != 0)//리스트에 뭐가있어야 실행
            {
                //초기화
                currentDist = 0f;
                closeDistIndex = 0;
                TargetIndex = -1;

                for (int i = 0; i < Player_Monster_List.Count; i++)//리스트에 있는 수만큼
                {
                    if (Player_Monster_List[i] == null) { return; }  //없으면 리턴

                    //하나씩 거리체크
                    currentDist = Vector3.Distance(transform.position, Player_Monster_List[i].transform.GetChild(0).position);

                    //거리에따라 제일 잚은것을 체크
                    if (TargetDist >= currentDist)
                    {
                        TargetIndex = i;

                        TargetDist = currentDist;

                    }
                    if (closetDist >= currentDist)
                    {
                        closeDistIndex = i;
                        closetDist = currentDist;
                    }

                }

                if (TargetIndex == -1)
                {
                    TargetIndex = closeDistIndex;
                }
                closetDist = 100f;
                TargetDist = 100f;
                IsTarget = true;
            }
            else
            {
                IsTarget = false;
            }
        }
    }

    //공격    
    void Attacking()
    {
        if (PlayerMove.Isruning)
        {
            IsAttack = false;
            return;
        }
        
        IsAttack = true;
        Quaternion angle = PlayerMove.PlayerRot;
        //앞으로
        GameObject copyTrickle = GameObject.Instantiate(Trickle, Trickle.transform.position, PlayerMove.PlayerRot);
        copyTrickle.SetActive(true);

        //대각선 발사
        if (IsDiagonally)
        {
            Quaternion Cor = Quaternion.Euler(new Vector3(0, 30, 0));
            angle = angle * Cor;
            copyTrickle = GameObject.Instantiate(Trickle, Trickle.transform.position, angle);
            copyTrickle.SetActive(true);


            Cor = Quaternion.Euler(new Vector3(0, -30, 0));
            angle = PlayerMove.PlayerRot;
            angle = angle * Cor;
            copyTrickle = GameObject.Instantiate(Trickle, Trickle.transform.position, angle);
            copyTrickle.SetActive(true);
        }
        //옆으로 발사
        if (IsSideway)
        {
            Quaternion cba = Quaternion.Euler(new Vector3(0, 90, 0));
            angle = PlayerMove.PlayerRot;
            angle = angle * cba;
            copyTrickle = GameObject.Instantiate(Trickle, Trickle.transform.position, angle);
            copyTrickle.SetActive(true);


            cba = Quaternion.Euler(new Vector3(0, -90, 0));
            angle = PlayerMove.PlayerRot;
            angle = angle * cba;
            copyTrickle = GameObject.Instantiate(Trickle, Trickle.transform.position, angle);
            copyTrickle.SetActive(true);
        }
        //뒤로발사
        if (IsBackway)
        {
            Quaternion cba = Quaternion.Euler(new Vector3(0, 180, 0));
            angle = PlayerMove.PlayerRot;
            angle = angle * cba;
            copyTrickle = GameObject.Instantiate(Trickle, Trickle.transform.position, angle);
            copyTrickle.SetActive(true);
        }
    }

    void FixedUpdate()
    {
        //타겟팅
        Targeting();
        if (!IsTarget)
        {
            IsAttack = false;
            return;
        }
        //가까운애를 바라봄
        transform.LookAt(Player_Monster_List[TargetIndex].transform.GetChild(0));

        //공격
        if (!IsFirst)
        {
            IsFirst = true;
            timer1 = DelaySec;
            for (int i = 1; i < Bulletnum + 1; i++)
            {
                Invoke("Attacking", 0.2f * i);
            }
        }

        timer1 -= Time.deltaTime;
        if(timer1 <= 0f)
        {
            timer1 = DelaySec;
            for(int i=1;i<Bulletnum+1;i++)
            {
                Invoke("Attacking", 0.2f * i);
            }
        }


    }
}
