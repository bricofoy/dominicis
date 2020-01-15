void mesures_attente()
{
  if (mesures.elapsed(2500))
    mesures.next(mesures_lectureCapteurs);
}

void mesures_lectureCapteurs()
{
  uint8_t i = mesures.runCount();

  if (i <= (sizeof(pinDHT) - 1))
    if (dht.read2(pinDHT[i], &T[i], &H[i], NULL) != SimpleDHTErrSuccess)
      T[i] = H[i] = ERREUR;

  if (i == (sizeof(pinDHT) - 1))
    mesures.next(mesures_attente);
}
