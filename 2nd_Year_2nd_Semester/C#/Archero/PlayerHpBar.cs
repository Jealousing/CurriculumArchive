using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class PlayerHpBar : MonoBehaviour
{
    //플레이어 hp 설정
    public Transform Player;
    public Slider Hpbar;
    public Text MyHp;

    // Update is called once per frame
    void Update()
    {
        transform.position = new Vector3(Player.position.x, Player.position.y + 3, Player.position.z+1);
        Hpbar.value = Mathf.Lerp(Hpbar.value, PlayerData.Player_Current_Hp / PlayerData.Player_Max_Hp, Time.deltaTime * 6.0f);
        MyHp.text = PlayerData.Player_Current_Hp.ToString();

    }

}
