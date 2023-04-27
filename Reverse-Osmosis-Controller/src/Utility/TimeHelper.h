namespace TimeHelper
{
  static time32_t GetTimeIfAble()
  {
    return Particle.connected() ? Time.now() : 0;
  }
}
