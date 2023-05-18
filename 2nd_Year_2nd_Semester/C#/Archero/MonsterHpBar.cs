using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class MonsterHpBar : MonoBehaviour
{
    //몬스터 hpbar 설정

    public Slider Hpbar;
    public Transform monster;

    float Current_Hp;
    float Max_Hp;

    
    void Update()
    {
        this.transform.position = new Vector3(monster.position.x, monster.position.y+2.0f, monster.position.z);
        //서서히감소
        Hpbar.value = Mathf.Lerp(Hpbar.value, Current_Hp / Max_Hp, Time.deltaTime * 4.0f);

        if(Current_Hp<=0)
        {
            Hpbar.gameObject.SetActive(false);
        }

    }
    public void HpSet(float _CurrHp,float _MaxHp)
    {
        Current_Hp = _CurrHp;
        Max_Hp = _MaxHp;
    }
}
