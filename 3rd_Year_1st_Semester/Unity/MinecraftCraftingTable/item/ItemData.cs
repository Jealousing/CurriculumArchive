using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(fileName = "New ItemData", menuName = "ItemData")]
public class ItemData : ScriptableObject
{
    [Header("아이템 아이콘")] public Sprite itemIcon;             //아이콘
    [Header("아이템 이름")] public string itemName;              //이름
    [Header("아이템 설명")] public string ItemTooltip;             //아이템설명
    [Header("아이템 타입")] public E_ItemType itemType;       //아이템 타입
    [Header("아이템 코드")] public E_ItemCode itemcode;       //아이템코드
}
