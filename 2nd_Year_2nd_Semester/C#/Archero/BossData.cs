using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BossData : MonoBehaviour
{
    //보스데이터 관리

    //최대 체력, 현제체력
    public float MaxHp = 10000;
    float CurHp;

    //보스의 hpbar, 죽일경우 승리메세지 text
    public GameObject Hpbar;
    public GameObject WinText;

    //중복 충돌(?) 오류 테스트용
    bool IsColl = false;

    //죽음
    bool IsDeath = false;
    
    //시작시 체력설정 체력바 설정
    void Start()
    {
        CurHp = MaxHp;
        Hpbar.GetComponent<BossHp>().HpSet(CurHp, MaxHp);
    }

    //체력 체크
    void HpCheck()
    {
        if (!IsDeath) //죽었는지
        {
            if (CurHp <= 0) // 0이하일경우 죽음
            {
                WinText.SetActive(true);        //승리메세지
                RoomSetting.IsRun = false;      //게임멈춤
                IsDeath = true;                             //죽음
                gameObject.SetActive(false);        //사라짐
                PlayerTargeting.Player_Monster_List.Remove(gameObject); //리스트에서삭제
            }
        }
    }

    //충돌
    void OnCollisionEnter(Collision collision)
    {
        if (IsColl)
            return;
        IsColl = true;
        if (collision.transform.CompareTag("Trickle"))//플레이어 물방울 
        {
            CurHp = CurHp - PlayerData.PlayerDamage;//데미지적용
            Hpbar.GetComponent<BossHp>().HpSet(CurHp, MaxHp);//hpbar수정
        }
    }
    //충돌벗어남
    void OnCollisionExit(Collision collision)
    {
        IsColl = false;
    }
  
    //수시로 체크
    void Update()
    {
        HpCheck();
    }
}
