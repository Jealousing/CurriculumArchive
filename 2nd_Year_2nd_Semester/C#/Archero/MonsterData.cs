using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MonsterData : MonoBehaviour
{
    //몬스터 체력
    public float MaxHp = 1000;
    float CurHp;

    //켄버스
    public GameObject _gameObject;

    
    bool IsColl = false;
    bool IsDeath = false;

    //체력바설정
    void Start()
    {
        CurHp = MaxHp;
        _gameObject.GetComponent<MonsterHpBar>().HpSet(CurHp, MaxHp);
    }

    //체력 체크
    void HpCheck()
    {
        if (!IsDeath)
        {
            if (CurHp <= 0)
            {
                IsDeath = true;
                gameObject.SetActive(false);
                PlayerTargeting.Player_Monster_List.Remove(gameObject);
            }
        }
        
    }
    //충돌
    void OnCollisionEnter(Collision collision)
    {
        if (IsColl)
            return;
        IsColl = true;
        if (collision.transform.CompareTag("Trickle"))
        {
            CurHp = CurHp - PlayerData.PlayerDamage;
            _gameObject.GetComponent<MonsterHpBar>().HpSet(CurHp, MaxHp);
        }
    }
    //충돌나감
    void OnCollisionExit(Collision collision)
    {
        IsColl = false;
    }

    void FixedUpdate()
    {
        HpCheck();
    }
}
