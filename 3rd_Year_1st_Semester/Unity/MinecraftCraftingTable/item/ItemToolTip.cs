using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

//툴팁 스크립트
public class ItemToolTip : MonoBehaviour
{
    //아이템 설명을 작성할 text 연결
    public Text m_ItemName;
    public Text m_ItemToolTip;
    public Text m_ItemCount;


    //툴팁생성
    public void GetToolTip(ItemData p_itemData, Vector3 p_pos, int p_itemnum)
    {
        //활성화
        this.gameObject.SetActive(true);

        //위치 설정
        this.gameObject.transform.position = p_pos += new Vector3(-this.gameObject.GetComponent<RectTransform>().rect.width, this.gameObject.GetComponent<RectTransform>().rect.height,0);

        //text 설정
        m_ItemName.text = p_itemData.itemName;
        m_ItemToolTip.text = p_itemData.ItemTooltip;

        //타입에 따른 아이탬 갯수 설정
        if (p_itemData.itemType == E_ItemType.Normal)
        {
            m_ItemCount.text = p_itemnum.ToString();
        }
        else
        {
            m_ItemCount.text = "";
        }
        
    }


    //숨기기
    public void HideToolTip()
    {
        this.gameObject.SetActive(false);
    }

}
