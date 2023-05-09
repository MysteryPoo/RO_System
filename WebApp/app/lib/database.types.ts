export type Json =
  | string
  | number
  | boolean
  | null
  | { [key: string]: Json }
  | Json[]

export interface Database {
  graphql_public: {
    Tables: {
      [_ in never]: never
    }
    Views: {
      [_ in never]: never
    }
    Functions: {
      graphql: {
        Args: {
          operationName?: string
          query?: string
          variables?: Json
          extensions?: Json
        }
        Returns: Json
      }
    }
    Enums: {
      [_ in never]: never
    }
    CompositeTypes: {
      [_ in never]: never
    }
  }
  public: {
    Tables: {
      component_list: {
        Row: {
          component_name: string
          description: string | null
          device_id: number
          display: string
          id: number
        }
        Insert: {
          component_name: string
          description?: string | null
          device_id: number
          display: string
          id?: number
        }
        Update: {
          component_name?: string
          description?: string | null
          device_id?: number
          display?: string
          id?: number
        }
      }
      device_list: {
        Row: {
          connected: boolean
          device_id: string
          device_name: string
          id: number
          inserted_at: string
          last_heard: string
          last_ip_address: string
          online: boolean
          version: string | null
        }
        Insert: {
          connected: boolean
          device_id: string
          device_name: string
          id?: number
          inserted_at?: string
          last_heard?: string
          last_ip_address: string
          online: boolean
          version?: string | null
        }
        Update: {
          connected?: boolean
          device_id?: string
          device_name?: string
          id?: number
          inserted_at?: string
          last_heard?: string
          last_ip_address?: string
          online?: boolean
          version?: string | null
        }
      }
      floatswitch_pin: {
        Row: {
          device_id: number
          id: number
          pin: number
        }
        Insert: {
          device_id: number
          id?: number
          pin: number
        }
        Update: {
          device_id?: number
          id?: number
          pin?: number
        }
      }
      floatswitch_reliable_reports: {
        Row: {
          datetime: string | null
          device_id: number
          id: number
          inserted_at: string
          reliable: boolean
        }
        Insert: {
          datetime?: string | null
          device_id: number
          id?: number
          inserted_at?: string
          reliable: boolean
        }
        Update: {
          datetime?: string | null
          device_id?: number
          id?: number
          inserted_at?: string
          reliable?: boolean
        }
      }
      floatswitch_status_reports: {
        Row: {
          datetime: string | null
          device_id: number
          id: number
          inserted_at: string
          status: boolean
        }
        Insert: {
          datetime?: string | null
          device_id: number
          id?: number
          inserted_at?: string
          status: boolean
        }
        Update: {
          datetime?: string | null
          device_id?: number
          id?: number
          inserted_at?: string
          status?: boolean
        }
      }
      log: {
        Row: {
          datetime: string | null
          device_id: number
          id: number
          inserted_at: string
          message: string
          severity: string
        }
        Insert: {
          datetime?: string | null
          device_id: number
          id?: number
          inserted_at?: string
          message: string
          severity: string
        }
        Update: {
          datetime?: string | null
          device_id?: number
          id?: number
          inserted_at?: string
          message?: string
          severity?: string
        }
      }
      option_boolean_list: {
        Row: {
          default: boolean
          id: number
          option_id: number
          value: boolean | null
        }
        Insert: {
          default: boolean
          id?: number
          option_id: number
          value?: boolean | null
        }
        Update: {
          default?: boolean
          id?: number
          option_id?: number
          value?: boolean | null
        }
      }
      option_list: {
        Row: {
          component_id: number
          id: number
          name: string
          type: string
        }
        Insert: {
          component_id: number
          id?: number
          name: string
          type: string
        }
        Update: {
          component_id?: number
          id?: number
          name?: string
          type?: string
        }
      }
      option_number_list: {
        Row: {
          default: number
          id: number
          option_id: number
          units: string | null
          value: number | null
        }
        Insert: {
          default: number
          id?: number
          option_id: number
          units?: string | null
          value?: number | null
        }
        Update: {
          default?: number
          id?: number
          option_id?: number
          units?: string | null
          value?: number | null
        }
      }
      profiles: {
        Row: {
          avatar_url: string | null
          full_name: string | null
          id: string
          updated_at: string | null
          username: string | null
          website: string | null
        }
        Insert: {
          avatar_url?: string | null
          full_name?: string | null
          id: string
          updated_at?: string | null
          username?: string | null
          website?: string | null
        }
        Update: {
          avatar_url?: string | null
          full_name?: string | null
          id?: string
          updated_at?: string | null
          username?: string | null
          website?: string | null
        }
      }
      restart: {
        Row: {
          datetime: string | null
          device_id: number
          id: number
          inserted_at: string
          reason: string
        }
        Insert: {
          datetime?: string | null
          device_id: number
          id?: number
          inserted_at?: string
          reason: string
        }
        Update: {
          datetime?: string | null
          device_id?: number
          id?: number
          inserted_at?: string
          reason?: string
        }
      }
      rosystem_enabled_reports: {
        Row: {
          datetime: string | null
          device_id: number
          enabled: boolean
          id: number
          inserted_at: string
        }
        Insert: {
          datetime?: string | null
          device_id: number
          enabled: boolean
          id?: number
          inserted_at?: string
        }
        Update: {
          datetime?: string | null
          device_id?: number
          enabled?: boolean
          id?: number
          inserted_at?: string
        }
      }
      rosystem_flushed_reports: {
        Row: {
          datetime: string | null
          device_id: number
          flushed: boolean
          id: number
          inserted_at: string
        }
        Insert: {
          datetime?: string | null
          device_id: number
          flushed: boolean
          id?: number
          inserted_at?: string
        }
        Update: {
          datetime?: string | null
          device_id?: number
          flushed?: boolean
          id?: number
          inserted_at?: string
        }
      }
      state_request: {
        Row: {
          datetime: string
          device_id: number
          failure_reason: string | null
          id: number
          request_reason: string | null
          state: string
          success: boolean
        }
        Insert: {
          datetime: string
          device_id: number
          failure_reason?: string | null
          id?: number
          request_reason?: string | null
          state: string
          success: boolean
        }
        Update: {
          datetime?: string
          device_id?: number
          failure_reason?: string | null
          id?: number
          request_reason?: string | null
          state?: string
          success?: boolean
        }
      }
      unknown_message: {
        Row: {
          created_at: string
          device_id: number
          id: number
          payload: string
          topic: string
        }
        Insert: {
          created_at?: string
          device_id: number
          id?: number
          payload: string
          topic: string
        }
        Update: {
          created_at?: string
          device_id?: number
          id?: number
          payload?: string
          topic?: string
        }
      }
      versions: {
        Row: {
          device_id: number | null
          id: number
          inserted_at: string
          version: string
        }
        Insert: {
          device_id?: number | null
          id?: number
          inserted_at?: string
          version: string
        }
        Update: {
          device_id?: number | null
          id?: number
          inserted_at?: string
          version?: string
        }
      }
      wifi_quality_reports: {
        Row: {
          datetime: string | null
          device_id: number | null
          id: number
          inserted_at: string
          quality: number
        }
        Insert: {
          datetime?: string | null
          device_id?: number | null
          id?: number
          inserted_at?: string
          quality: number
        }
        Update: {
          datetime?: string | null
          device_id?: number | null
          id?: number
          inserted_at?: string
          quality?: number
        }
      }
      wifi_signal_reports: {
        Row: {
          datetime: string | null
          device_id: number
          id: number
          inserted_at: string
          signal: number
        }
        Insert: {
          datetime?: string | null
          device_id: number
          id?: number
          inserted_at?: string
          signal: number
        }
        Update: {
          datetime?: string | null
          device_id?: number
          id?: number
          inserted_at?: string
          signal?: number
        }
      }
    }
    Views: {
      [_ in never]: never
    }
    Functions: {
      [_ in never]: never
    }
    Enums: {
      [_ in never]: never
    }
    CompositeTypes: {
      [_ in never]: never
    }
  }
  storage: {
    Tables: {
      buckets: {
        Row: {
          allowed_mime_types: string[] | null
          avif_autodetection: boolean | null
          created_at: string | null
          file_size_limit: number | null
          id: string
          name: string
          owner: string | null
          public: boolean | null
          updated_at: string | null
        }
        Insert: {
          allowed_mime_types?: string[] | null
          avif_autodetection?: boolean | null
          created_at?: string | null
          file_size_limit?: number | null
          id: string
          name: string
          owner?: string | null
          public?: boolean | null
          updated_at?: string | null
        }
        Update: {
          allowed_mime_types?: string[] | null
          avif_autodetection?: boolean | null
          created_at?: string | null
          file_size_limit?: number | null
          id?: string
          name?: string
          owner?: string | null
          public?: boolean | null
          updated_at?: string | null
        }
      }
      migrations: {
        Row: {
          executed_at: string | null
          hash: string
          id: number
          name: string
        }
        Insert: {
          executed_at?: string | null
          hash: string
          id: number
          name: string
        }
        Update: {
          executed_at?: string | null
          hash?: string
          id?: number
          name?: string
        }
      }
      objects: {
        Row: {
          bucket_id: string | null
          created_at: string | null
          id: string
          last_accessed_at: string | null
          metadata: Json | null
          name: string | null
          owner: string | null
          path_tokens: string[] | null
          updated_at: string | null
          version: string | null
        }
        Insert: {
          bucket_id?: string | null
          created_at?: string | null
          id?: string
          last_accessed_at?: string | null
          metadata?: Json | null
          name?: string | null
          owner?: string | null
          path_tokens?: string[] | null
          updated_at?: string | null
          version?: string | null
        }
        Update: {
          bucket_id?: string | null
          created_at?: string | null
          id?: string
          last_accessed_at?: string | null
          metadata?: Json | null
          name?: string | null
          owner?: string | null
          path_tokens?: string[] | null
          updated_at?: string | null
          version?: string | null
        }
      }
    }
    Views: {
      [_ in never]: never
    }
    Functions: {
      can_insert_object: {
        Args: {
          bucketid: string
          name: string
          owner: string
          metadata: Json
        }
        Returns: undefined
      }
      extension: {
        Args: {
          name: string
        }
        Returns: string
      }
      filename: {
        Args: {
          name: string
        }
        Returns: string
      }
      foldername: {
        Args: {
          name: string
        }
        Returns: unknown
      }
      get_size_by_bucket: {
        Args: Record<PropertyKey, never>
        Returns: {
          size: number
          bucket_id: string
        }[]
      }
      search: {
        Args: {
          prefix: string
          bucketname: string
          limits?: number
          levels?: number
          offsets?: number
          search?: string
          sortcolumn?: string
          sortorder?: string
        }
        Returns: {
          name: string
          id: string
          updated_at: string
          created_at: string
          last_accessed_at: string
          metadata: Json
        }[]
      }
    }
    Enums: {
      [_ in never]: never
    }
    CompositeTypes: {
      [_ in never]: never
    }
  }
}
