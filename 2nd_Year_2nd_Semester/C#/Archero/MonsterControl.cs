using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

public class MonsterControl : MonoBehaviour
{
    public Transform TargetTrans; //목표
    public GameObject Trickle;         //발사체

    public float delayTime = 3f;        //딜레이
    public float SaveTime = 1f;         //공격시간
    float MoveTime = 3f;                   //이동시간
    public bool MoveControl = false;    //컨트롤

    float RandomTime = 0.0f;            //랜덤용 시간

    bool OnOff = false;                 //onoff스위치

    float SaveSpeed = 0;            //스피드저장용
       
    //라운드
    public int Round;

    //네비
    public NavMeshAgent LinkNavMesh = null;

    //총알정보
    public int Bulletnum = 1;
    public bool IsSideway = false;
    public bool IsBackway = false;
    public bool IsDiagonally = false;
    bool isattack = false;

    void Start()
    {
        //설정
        RandomTime = Random.Range(0.0f, 6.0f);
        LinkNavMesh = GetComponent<NavMeshAgent>();
        SaveSpeed = LinkNavMesh.speed;
        MoveTime = RandomTime;
    }

    void UpdateMove()
    {
        //딜레이 이후에 움직임
        if(MoveControl&&!RoomSetting.IsRoom[NextRound.Round]&& NextRound.Round == Round)
        {
            if (delayTime > 0)
                delayTime -= 1 * Time.deltaTime;
            else
            {
                LinkNavMesh.SetDestination(TargetTrans.transform.position);
                LinkNavMesh.speed = SaveSpeed;
            }
        }
    }

    void attack()
    {
        //스피드 조정 공격할땐 멈춤
        LinkNavMesh.speed = 0;

        //기본 각도
        Quaternion angle = this.transform.rotation;
    
        //앞으로 발사
        GameObject copyTrickle = GameObject.Instantiate(Trickle, Trickle.transform.position, angle);
        copyTrickle.SetActive(true);


        // 대각성 30도 양옆 발사
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
            Quaternion Cor = Quaternion.Euler(new Vector3(0, 90, 0));
            angle = PlayerMove.PlayerRot;
            angle = angle * Cor;
            copyTrickle = GameObject.Instantiate(Trickle, Trickle.transform.position, angle);
            copyTrickle.SetActive(true);


            Cor = Quaternion.Euler(new Vector3(0, -90, 0));
            angle = PlayerMove.PlayerRot;
            angle = angle * Cor;
            copyTrickle = GameObject.Instantiate(Trickle, Trickle.transform.position, angle);
            copyTrickle.SetActive(true);
        }
        //뒤로 발사
        if (IsBackway)
        {
            Quaternion Cor = Quaternion.Euler(new Vector3(0, 180, 0));
            angle = PlayerMove.PlayerRot;
            angle = angle * Cor;
            copyTrickle = GameObject.Instantiate(Trickle, Trickle.transform.position, angle);
            copyTrickle.SetActive(true);
        }
    }



    void FixedUpdate()
    {
        //이동과 공격 방식

        if (!RoomSetting.IsRun)
            return;
        if (NextRound.Round != Round)
            return;

        if (MoveTime>0)
        {
            MoveTime -= 1 * Time.deltaTime;
           
        }
        else
        {
            if (OnOff)
            {
                MoveTime = SaveTime;
                OnOff = false;
                isattack = false;
            }
            else
            {
                MoveTime = RandomTime;
                OnOff = true;
            }
        }
       
        if (OnOff)
        {
            if (isattack)
                return;
            if(!MoveControl)
            {
                this.transform.LookAt(TargetTrans);
            }
            for (int i = 1; i < Bulletnum + 1; i++)
            {
                Invoke("attack", 0.2f * i);
            }
            isattack = true;

            Invoke("UpdateMove", 0.5f);
        }
        else
        {
            UpdateMove();
        }
       
        
    }
}
