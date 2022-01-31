class GenericAPI {
  async getVersion(): Promise<string> {
    const request = await fetch(`http://${window.location.hostname}:4000/version`);
    if (request.status === 200) {
      return request.text();
    }
    return 'Unknown';
  }
}

export function useGenericAPI(): GenericAPI {
  return new GenericAPI();
}
