// TODO : Maybe this API should take a token as a parameter rather than grabbing it from the localStorage itself?


export class UnauthorizedException extends Error {
  code = 401;
  message = "Unauthorized";
}

export class DeviceRequiredException extends Error {
  code = 422;
  message = "No device provided";
}

class DevicesApi {

  async getDeviceList() {
    const response = await fetch(`http://${window.location.hostname}:4000/deviceList`);
    if (response.status === 200) {
      return response.json();
    }
    return [];
  }

  async getConfiguration(deviceId: string | undefined) {
    if (deviceId === undefined) {
      throw new DeviceRequiredException();
    }
    const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/configuration`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      return response.json();
    }
    if (response.status === 401) {
      throw new UnauthorizedException();
    }
  }

  async setConfiguration(deviceId: string | undefined, configuration : any) {
    if (deviceId === undefined) {
      throw new DeviceRequiredException();
    }
    const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/configuration`, {
      method: 'POST',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
        Accept: 'application/json',
        'Content-Type': 'application/json'
      },
      body: JSON.stringify(configuration),
    });
    if (response.status === 201) {
      return response.json();
    }
    if (response.status === 401) {
      throw new UnauthorizedException();
    }
    return {
      success: false,
      code: response.status,
    };
  }

  async getStatus(deviceId: string | undefined) {
    if (deviceId === undefined) {
      throw new DeviceRequiredException();
    }
    const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/status`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      return response.json();
    }
    if (response.status === 401) {
      throw new UnauthorizedException();
    }
  }

  async getTicks(deviceId: string | undefined, skip = 0, rows = 1) {
    if (deviceId === undefined) {
      return [];
    }
    const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/heartbeat?skip=${skip}&rows=${rows}`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      return response.json();
    }
    if (response.status === 401) {
      throw new UnauthorizedException();
    }
  }

  async getHeartbeat(deviceId: string | undefined, skip = 0, rows = 1) : Promise<Array<any>> {
    return this.getTicks(deviceId, skip, rows);
  }

  async getFeatureList(deviceId: string | undefined) {
    if (deviceId === undefined) {
      return [];
    }
    const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/status/feature-list`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      return response.json();
    }
    if (response.status === 401) {
      throw new UnauthorizedException();
    }
  }

  async getStatesCount(deviceId: string | undefined) {
    if (deviceId === undefined) {
      return 0;
    }
    const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/states?count=true`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      return response.json();
    }
    if (response.status === 401) {
      throw new UnauthorizedException();
    }
  }

  async getStates(deviceId: string | undefined, skip = 0, rows = 10, stateFilter = ['IDLE', 'FILL', 'FLUSH']) {
    if (deviceId === undefined) {
      throw new DeviceRequiredException();
    }
    const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/states?states=${JSON.stringify(stateFilter)}&skip=${skip}&rows=${rows}`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      return response.json();
    }
    if (response.status === 401) {
      throw new UnauthorizedException();
    }
  }

  async getLogsCount(deviceId: string | undefined) {
    if (deviceId === undefined) {
      return 0;
    }
    const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/logs?count=true`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      return response.json();
    }
    if (response.status === 401) {
      throw new UnauthorizedException();
    }
  }

  async getLogs(deviceId: string | undefined, skip = 0) {
    if (deviceId === undefined) {
      throw new DeviceRequiredException();
    }
    const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/logs?skip=${skip}`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      return response.json();
    }
    if (response.status === 401) {
      throw new UnauthorizedException();
    }
  }

  async clearLog(deviceId: string | undefined, logId: string) {
    if (deviceId === undefined) {
      throw new DeviceRequiredException();
    }
    const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/logs/${logId}`, {
      method: 'DELETE',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 401) {
      throw new UnauthorizedException();
    }
    return response;
  }

  async getRestarts(deviceId: string | undefined) {
    if (deviceId === undefined) {
      throw new DeviceRequiredException();
    }
    const request = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/restart`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (request.status === 200) {
      return request.json();
    }
    if (request.status === 401) {
      throw new UnauthorizedException();
    }
  }
}

export function useDevicesApi() : DevicesApi {
  return new DevicesApi();
}
