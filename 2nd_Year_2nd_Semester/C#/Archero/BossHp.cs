using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class BossHp : MonoBehaviour
{
    //보스의 hp바설정
    public Slider BossHpBar;
    //이름
    public Text BossName;

    //켄버스
    public GameObject _gameObject;

    //체력
    float Current_Hp;
    float Max_Hp;
    void Start()
    {
        //혹시모를 비활성화
        BossName.gameObject.SetActive(false);
        BossHpBar.gameObject.SetActive(false);
    }

    // Update is called once per frame
    void Update()
    {
        //라운드체크
        if(NextRound.Round==10)
        {
            //활성화
            BossName.gameObject.SetActive(true);
            BossHpBar.gameObject.SetActive(true);

            //hpbar가 서서히 줄어듬
            BossHpBar.value = Mathf.Lerp(BossHpBar.value, Current_Hp / Max_Hp, Time.deltaTime * 4.0f);
            if (Current_Hp <= 0)
            {
                //죽으면 비활성화
                BossName.gameObject.SetActive(false);
                BossHpBar.gameObject.SetActive(false);
            }
        }
    }

    //hp세팅
    public void HpSet(float _CurrHp, float _MaxHp)
    {
        Current_Hp = _CurrHp;
        Max_Hp = _MaxHp;
    }
}
