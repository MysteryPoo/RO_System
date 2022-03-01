export interface IFeature {
  deviceId: string;
  component: string;
  display: string;
  description: string;
  options: Array<{ name: string; type: string; units?: string; default?: any }>;
}
