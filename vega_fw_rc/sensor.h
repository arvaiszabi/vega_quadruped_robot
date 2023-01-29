void check_distance()
{
  int i = 0; //Kiolvasások számlálása
  int sum = 0; //Értékek gyűjtése átlagoláshoz.
  for(; i < DIST_RD_COUNT; ++i)
  {
    sum += analogRead(DIST_SNSR);
  }
  Serial.println(distance + (sum/i));
}
void check_battery()
{
  int i = 0;
  int sum = 0;
  for(;i < BAT_RD_COUNT; ++i)
  {
    sum += analogRead(BAT_SNSR);
  }
  Serial.println(battery + (sum/i));
}
void check_light()
{
  int i = 0;
  int sum = 0;
  for(;i < LIGT_RD_COUNT; ++i)
  {
    sum += analogRead(LIGT_SNSR);
  }
  Serial.println(light + (sum/i));
}
