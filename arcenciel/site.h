#ifndef ARC_SITE_H
#define ARC_SITE_H

/*---- �����Ȥξ��� ----*/
enum siteState {  UNOCCUPY, OCCUPY};

/*---- �����Ⱦ��� ----*/
struct siteInformation{
  unsigned long num;                /* �ֹ� */
  struct siteTypeInformation *type; /* ���� */
  pos3D pos;                        /* ��ɸ */
  enum siteState state;             /* ���� */
  int numNeighbor;                  /* ���ܤ��륵���Ȥο� */
  struct siteInformation **neighbor; /* ���ܤ��륵���ȤΥݥ��󥿤����� */
  struct pathTypeInformation **pathTypeToNeighbor;
};

/*---- �����Ȥμ��� ----*/
struct siteTypeInformation{
  int num;
  char name[NAME_LIMIT];
};

#endif
