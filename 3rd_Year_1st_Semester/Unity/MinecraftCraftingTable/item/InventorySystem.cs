using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class InventorySystem : MonoBehaviour
{
    /*
     해당 인벤토리에 비어있는 곳을 찾아 아이템의 정보를 넘겨주는 역활
     */

    //인벤토리 오브젝트 연결
    public GameObject m_InventorySystem;
    public GameObject m_FavoriteInventory;
    public GameObject m_Inventory;

    //아이템 데이터 설정
    public void SetItem(ItemData p_itemData, int p_itemNum)
    {
        //메인인벤토리의 빈자리를 찾았는지 확인용
        bool searchvoid = false ;

        //즐겨찾기 인벤토리에서 빈공간을 찾기
        for (int i=0;i< m_FavoriteInventory.transform.childCount;i++)
        {
            //빈공간을 찾으면
            if(m_FavoriteInventory.transform.GetChild(i).GetComponent<ItemSlotControl>().m_itemData==null)
            {
                //아이템 슬롯에 아이템 데이터와 아이템 갯수를 넣어주고 슬롯을 업데이트
                m_FavoriteInventory.transform.GetChild(i).GetComponent<ItemSlotControl>().m_itemData = p_itemData;
                m_FavoriteInventory.transform.GetChild(i).GetComponent<ItemSlotControl>().m_itemNum = p_itemNum;
                m_FavoriteInventory.transform.GetChild(i).GetComponent<ItemSlotControl>().UpdateSlotData();
                searchvoid = true;
                break;
            }
        }

        //위의 즐겨찾기인벤토리에서 빈공간을 못찾았으면 다른 인벤토리칸에서 찾기시작
        if(!searchvoid)
        {
            //서브 인벤토리를 돌면서 빈공간을 찾는다
            for(int i=0;i<m_Inventory.transform.childCount;i++)
            {
                //인벤토리의 빈공간을 찾게되면 진행
                if(m_Inventory.transform.GetChild(i).GetComponent<ItemSlotControl>().m_itemData==null)
                {
                    //아이템 슬롯에 아이템 데이터와 아이템 갯수를 넣어주고 슬롯을 업데이트
                    m_Inventory.transform.GetChild(i).GetComponent<ItemSlotControl>().m_itemData = p_itemData;
                    m_Inventory.transform.GetChild(i).GetComponent<ItemSlotControl>().m_itemNum = p_itemNum;
                    m_Inventory.transform.GetChild(i).GetComponent<ItemSlotControl>().UpdateSlotData();
                    break;
                }
            }
        }
    }


}
