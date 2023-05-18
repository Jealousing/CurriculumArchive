using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RecipeManager : MonoBehaviour
{

    public ItemSlotControls_Array[] m_CraftingSlots = new ItemSlotControls_Array[3];    //제작창
    public ItemSlotControl m_GetItemSlot;                                                                           //결과창
    private List<RecipeData> m_Recipes = new List<RecipeData>();                                      //레시피 저장
    bool m_RecipeSearch;                                                                                                     //찾아야되는지 확인

    void Start()
    {
        //레시피받아옴
        m_Recipes.AddRange(Resources.LoadAll<RecipeData>("RecipeData"));
    }

    //레시피를 찾아야하는지
    public void SetRecipeBool()
    {
        m_RecipeSearch = true;
    }
    //제작 완료 실행
    public void CraftingComplete()
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                //제작창에 데이터가 있을경우
                if (m_CraftingSlots[i].array[j].m_itemData != null)
                {
                    //갯수를 줄인다
                    if (m_CraftingSlots[i].array[j].m_itemNum > 1)
                    {
                        m_CraftingSlots[i].array[j].m_itemNum = m_CraftingSlots[i].array[j].m_itemNum - 1;
                        m_CraftingSlots[i].array[j].UpdateSlotData();
                    }
                    else //1개일 경우 0개로 만들면서 데이터 삭제
                    {
                        m_CraftingSlots[i].array[j].m_itemData = null;
                        m_CraftingSlots[i].array[j].m_itemNum = 0;
                        m_CraftingSlots[i].array[j].UpdateSlotData();
                    }


                }
            }
        }
    }

    
    void Update()
    {
        //레시피를 찾아야되는지 확인
        if (!m_RecipeSearch)
            return;

        //레시피리스트에 저장된 모든 데이터 확인
        foreach (RecipeData recipe in m_Recipes)
        {
            //결과
            bool result = true;

            //Looks at each crafting row
            for (int i = 0; i < recipe.RecipeArray.Length; i++)
            {
                for (int j = 0; j < recipe.RecipeArray[i].array.Length; j++)
                {
                    //레시피 해당 공간에 데이터가 있으면
                    if (recipe.RecipeArray[i].array[j] != null)
                    {
                        //해당 크래프트 슬롯에 데이터가 있으면
                        if (m_CraftingSlots[i].array[j].m_itemData != null)
                        {
                            //해당 데이터가 다르면
                            if (recipe.RecipeArray[i].array[j].itemcode != m_CraftingSlots[i].array[j].m_itemData.itemcode)
                            {
                                //이 조합법이 아님
                                result = false;
                            }
                        }
                        else
                        {//이 조합법이 아님
                            result = false;
                        }
                    }
                    else
                    {
                        //레시피에는 데이터가 없는데 제작창엔 데이터가 있으면 false
                        if (m_CraftingSlots[i].array[j].m_itemData != null)
                        {
                            //이 조합법이 아님
                            result = false;
                            continue;
                        }
                    }
                }
            }

            //조합법이 있을경우
            if (result)
            {
                //결과창에 아이템 데이터와 결과 갯수 넣기 및 슬롯 업데이트
                m_GetItemSlot.m_itemData = recipe.GetItem;
                m_GetItemSlot.m_itemNum = recipe.GetNum;
                m_GetItemSlot.UpdateSlotData();
                m_RecipeSearch = false; //찾았으니 끝
                break;
            }
            else
            {
                //조합법 없으면 데이터가 있을일이 없으니 삭제
                m_GetItemSlot.m_itemData = null;
                m_GetItemSlot.UpdateSlotData();
            }
        }
    }
}
