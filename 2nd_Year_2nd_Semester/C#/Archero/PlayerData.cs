using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerData : MonoBehaviour
{
    //패배 text
    public GameObject LoseText;

    //플레이어 대미지, 현재 체력 , 최대 체력 , 바운스 횟수
    public static float PlayerDamage = 200.0f;
    public static float Player_Current_Hp = 1000.0f;
    public static float Player_Max_Hp = 1000.0f;
    public static int BounceNum = 0;

    private void FixedUpdate()
    {
        //사망처리
        if(Player_Current_Hp<=0)
        {
            LoseText.SetActive(true);
            RoomSetting.IsRun = false;
        }
            
    }


}
