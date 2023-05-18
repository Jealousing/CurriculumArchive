using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RoomSetting : MonoBehaviour
{
    //룸세팅
    
    //스킬
    public GameObject _skill;
    //몬스터리스트
    public List<GameObject> Room_Monster_List = new List<GameObject>();
    //룸정보
    public static bool[] IsRoom = new bool[20];
    public static bool[] isClear = new bool[20];
    //라운드
    public int Round;

    //게임진행여부
    public static bool IsRun = false;

    //레벨업
    bool IsLVUP = false;

    void Start()
    {
        IsRun = true;
    }

    void Update()
    {
        //클리어 체크
        if (IsRoom[NextRound.Round])
        {
            if (PlayerTargeting.Player_Monster_List.Count <= 0 && !isClear[NextRound.Round])
            {
               
                isClear[NextRound.Round] = true;
                Debug.Log(" Clear ");
            }
        }
        //클리어 할때 레벨업설정
        if (PlayerTargeting.Player_Monster_List.Count <= 0 && !IsLVUP)
        {
            IsRun = false;
            if(Round!=10)
                _skill.gameObject.SetActive(true);
            

            if (PlayerData.Player_Max_Hp>=PlayerData.Player_Current_Hp+100)
            {
                PlayerData.Player_Current_Hp += 100;
            }
            else
            {
                PlayerData.Player_Current_Hp = PlayerData.Player_Max_Hp;
            }
            
            IsLVUP = true;
        }
    }

    //플레이어가 방에들어오면 리스트 추가
    void OnCollisionEnter(Collision collision)
    {
        if (collision.transform.CompareTag("Player"))
        {
            NextRound.Round = Round;
            IsLVUP = false;
            IsRoom[NextRound.Round] = true;
            PlayerTargeting.Player_Monster_List = new List<GameObject>(Room_Monster_List);
        }
    }

    //exit설정
    void OnCollisionExit(Collision collision)
    {
        if (collision.transform.CompareTag("Player"))
        {
            IsRoom[NextRound.Round] = false;
        }

        if (collision.transform.CompareTag("Monster"))
        {
            Room_Monster_List.Remove(collision.gameObject);
        }
    }

}
